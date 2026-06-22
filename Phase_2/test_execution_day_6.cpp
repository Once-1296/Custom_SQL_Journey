// test_day6.cpp
#include "my_custom_disk_manager.hpp"
#include "my_custom_buffer_pool_manager.hpp"
#include "executor_context.hpp"
#include "seq_scan_executor.hpp"
#include "filter_executor.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <array>

void TestFilterPipeline() {
    std::string filename = "test_filter_db.bin";
    std::remove(filename.c_str());

    DiskManager disk(filename);
    BufferPoolManager bpm(disk);
    ExecutorContext exec_ctx(bpm);

    // Schema: { id (int32), username (varchar), age (int32) }
    std::vector<Column> cols = {
        {"id",       TypeId::INT32,   4, 0},
        {"username", TypeId::VARCHAR, 32, 0},
        {"age",      TypeId::INT32,   4, 0}
    };
    Schema schema(cols);
    uint32_t tuple_size = schema.GetTupleSize();

    // Serialize data: Alice (Age 25), Bob (Age 30)
    uint32_t p0_id = disk.AllocatePage();
    Page* p0 = bpm.FetchPage(p0_id);

    std::array<uint8_t, 40> alice_buf{0};
    int32_t id1 = 1; int32_t age1 = 25;
    std::memcpy(alice_buf.data() + 0, &id1, 4);
    std::memcpy(alice_buf.data() + 4, "alice", 5);
    std::memcpy(alice_buf.data() + 36, &age1, 4);
    p0->AppendRecord(alice_buf.data(), tuple_size);

    std::array<uint8_t, 40> bob_buf{0};
    int32_t id2 = 2; int32_t age2 = 30;
    std::memcpy(bob_buf.data() + 0, &id2, 4);
    std::memcpy(bob_buf.data() + 4, "bob", 3);
    std::memcpy(bob_buf.data() + 36, &age2, 4);
    p0->AppendRecord(bob_buf.data(), tuple_size);
    bpm.MarkDirty(p0_id);

    // Build the Pipeline
    // Child Scan
    auto scan_exec = std::make_unique<SeqScanExecutor>(&exec_ctx, schema, disk.GetTotalPages());
    
    // Predicate: age (col idx 2) > 28
    auto col_expr = std::make_unique<ColumnValueExpression>(2);
    auto const_expr = std::make_unique<ConstantValueExpression>(Value(28));
    auto predicate = std::make_unique<ComparisonExpression>(std::move(col_expr), std::move(const_expr));

    // Parent Filter Parent
    FilterExecutor filter_exec(&exec_ctx, std::move(scan_exec), std::move(predicate));
    filter_exec.Init();

    Tuple t; RID r;

    // First call to Next() should bypass Alice (25) and yield Bob (30)
    assert(filter_exec.Next(&t, &r) == true);
    assert(t.GetInt32(schema, 0) == 2); // Bob's ID
    assert(t.GetInt32(schema, 2) == 30); // Bob's Age

    // Second call to Next() should result in EOF (false)
    assert(filter_exec.Next(&t, &r) == false);

    std::cout << "Day 6 Filter Pipeline Verification Passed!" << std::endl;
    std::remove(filename.c_str());
}

int main() {
    TestFilterPipeline();
    return 0;
}