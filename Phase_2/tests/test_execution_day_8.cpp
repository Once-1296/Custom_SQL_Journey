// test_day8.cpp
#include "../src/storage/my_custom_disk_manager.hpp"
#include "../src/storage/my_custom_buffer_pool_manager.hpp"
#include "../src/executor_context.hpp"
#include "../src/executors/seq_scan_executor.hpp"
#include "../src/executors/filter_executor.hpp"
#include "../src/executors/projection_executor.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#include <array>

void TestEndToEndPipeline()
{
    std::string filename = "end_to_end_db.bin";
    std::remove(filename.c_str());

    DiskManager disk(filename);
    BufferPoolManager bpm(disk);
    ExecutorContext exec_ctx(bpm);

    // 1. Core Source Layout: { id (0), username (1), age (2) }
    std::vector<Column> src_cols = {
        {"id", TypeId::INT32, 4, 0},
        {"username", TypeId::VARCHAR, 32, 0},
        {"age", TypeId::INT32, 4, 0}};
    Schema src_schema(src_cols);
    uint32_t tuple_size = src_schema.GetTupleSize();

    // 2. Populate Disk Frame: Create 5 users spread over 3 pages
    uint32_t p0_id = disk.AllocatePage();
    uint32_t p1_id = disk.AllocatePage();
    uint32_t p2_id = disk.AllocatePage();

    // Page 0: Alice (ID: 1, Age: 25), Bob (ID: 2, Age: 30)
    Page *p0 = bpm.FetchPage(p0_id);
    std::array<uint8_t, 40> alice{0};
    int32_t id1 = 1;
    int32_t age1 = 25;
    std::memcpy(alice.data() + 0, &id1, 4);
    std::memcpy(alice.data() + 4, "alice", 5);
    std::memcpy(alice.data() + 36, &age1, 4);
    p0->AppendRecord(alice.data(), tuple_size);

    std::array<uint8_t, 40> bob{0};
    int32_t id2 = 2;
    int32_t age2 = 30;
    std::memcpy(bob.data() + 0, &id2, 4);
    std::memcpy(bob.data() + 4, "bob", 3);
    std::memcpy(bob.data() + 36, &age2, 4);
    p0->AppendRecord(bob.data(), tuple_size);
    bpm.MarkDirty(p0_id);

    // Page 1: Charlie (ID: 3, Age: 35)
    Page *p1 = bpm.FetchPage(p1_id);
    std::array<uint8_t, 40> charlie{0};
    int32_t id3 = 3;
    int32_t age3 = 35;
    std::memcpy(charlie.data() + 0, &id3, 4);
    std::memcpy(charlie.data() + 4, "charlie", 7);
    std::memcpy(charlie.data() + 36, &age3, 4);
    p1->AppendRecord(charlie.data(), tuple_size);
    bpm.MarkDirty(p1_id);

    Page *p2 = bpm.FetchPage(p2_id);
    std::array<uint8_t, 40> donovan{0};
    int32_t id4 = 4;
    int32_t age4 = 40;
    std::memcpy(donovan.data() + 0, &id4, 4);
    std::memcpy(donovan.data() + 4, "donovan", 7);
    std::memcpy(donovan.data() + 36, &age4, 4);
    p2->AppendRecord(donovan.data(), tuple_size);

    std::array<uint8_t, 40> eve{0};
    int32_t id5 = 2;
    int32_t age5 = 23;
    std::memcpy(eve.data() + 0, &id5, 4);
    std::memcpy(eve.data() + 4, "eve", 3);
    std::memcpy(eve.data() + 36, &age5, 4);
    p2->AppendRecord(eve.data(), tuple_size);
    bpm.MarkDirty(p2_id);

    // 3. TARGET PROJECTION SCHEMA: { age (0), id (1) }
    std::vector<Column> dest_cols = {
        {"age", TypeId::INT32, 4, 0},
        {"id", TypeId::INT32, 4, 0}};
    Schema dest_schema(dest_cols);

    // ==========================================
    // WIRE UP THE VOLCANO PIPELINE TREE
    // ==========================================

    // Step A: Instantiate the base sequential scanner unique pointer
    auto scan_node = std::make_unique<SeqScanExecutor>(&exec_ctx, src_schema, disk.GetTotalPages());

    // Step B: Formulate the filter predicate condition (age > 26)
    auto col_expr = std::make_unique<ColumnValueExpression>(2); // age index
    auto const_expr = std::make_unique<ConstantValueExpression>(Value(26));
    auto filter_predicate = std::make_unique<ComparisonExpression>(std::move(col_expr), std::move(const_expr));

    // Step C: Wrap the scan node inside the Filter Executor unique pointer
    auto filter_node = std::make_unique<FilterExecutor>(&exec_ctx, std::move(scan_node), std::move(filter_predicate));

    // Step D: Map out the projection remapping parameters
    std::vector<std::unique_ptr<AbstractExpression>> proj_exprs;
    proj_exprs.push_back(std::make_unique<ColumnValueExpression>(2)); // Map source age to dest col 0
    proj_exprs.push_back(std::make_unique<ColumnValueExpression>(0)); // Map source id to dest col 1

    // Step E: Wrap the filter node inside the final root Projection Executor node
    // YOUR CODE HERE: Initialize the projection plan coordinator node
    ProjectionExecutor plan_root(&exec_ctx, std::move(filter_node), dest_schema, std::move(proj_exprs));

    // Initialize root execution pipeline
    plan_root.Init();

    Tuple t;
    RID r;

    // First execution iteration: Should filter Alice (25), skip to Bob (30), and drop "username"
    assert(plan_root.Next(&t, &r) == true);
    assert(t.GetInt32(dest_schema, 0) == 30); // Bob's Age mapped to index 0
    assert(t.GetInt32(dest_schema, 1) == 2);  // Bob's ID mapped to index 1

    // Second execution iteration: Should smoothly cross physical disk blocks to yield Charlie (35)
    assert(plan_root.Next(&t, &r) == true);
    assert(t.GetInt32(dest_schema, 0) == 35); // Charlie's Age mapped to index 0
    assert(t.GetInt32(dest_schema, 1) == 3);  // Charlie's ID mapped to index 1

    // Third execution iteration: Should smoothly cross physical disk blocks to yield Donovan (40)
    assert(plan_root.Next(&t, &r) == true);
    assert(t.GetInt32(dest_schema, 0) == 40); // Donovan's Age mapped to index 0
    assert(t.GetInt32(dest_schema, 1) == 4);  // Donovan's ID mapped to index 1
    // Final call: Plan execution complete (EOF)
    assert(plan_root.Next(&t, &r) == false);

    std::cout << "DAY 8 END-TO-END PIPELINE WORKS PERFECTLY! PHASE 2 COMPLETE!" << std::endl;
    std::remove(filename.c_str());
}

int main()
{
    TestEndToEndPipeline();
    return 0;
}