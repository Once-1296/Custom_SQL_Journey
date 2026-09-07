// test_day7.cpp
#include "../src/storage/my_custom_disk_manager.hpp"
#include "../src/storage/my_custom_buffer_pool_manager.hpp"
#include "../src/executor_context.hpp"
#include "../src/executors/seq_scan_executor.hpp"
#include "../src/executors/projection_executor.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <array>

void TestProjectionPipeline() {
    std::string filename = "test_projection_db.bin";
    std::remove(filename.c_str());

    DiskManager disk(filename);
    BufferPoolManager bpm(disk);
    ExecutorContext exec_ctx(bpm);

    // Source Schema (3 Columns): { id, username, age }
    std::vector<Column> src_cols = {
        {"id",       TypeId::INT32,   4, 0},
        {"username", TypeId::VARCHAR, 32, 0},
        {"age",      TypeId::INT32,   4, 0}
    };
    Schema src_schema(src_cols);

    // Write a physical mock record for Alice (ID: 99, Age: 27)
    uint32_t p0_id = disk.AllocatePage();
    Page* p0 = bpm.FetchPage(p0_id);
    std::array<uint8_t, 40> alice_raw{0};
    int32_t id = 99; int32_t age = 27;
    std::memcpy(alice_raw.data() + 0, &id, 4);
    std::memcpy(alice_raw.data() + 4, "alice", 5);
    std::memcpy(alice_raw.data() + 36, &age, 4);
    p0->AppendRecord(alice_raw.data(), src_schema.GetTupleSize());
    bpm.MarkDirty(p0_id);

    // Target Output Schema (2 Columns, Reordered): { age, id }
    std::vector<Column> dest_cols = {
        {"age", TypeId::INT32, 4, 0},
        {"id",  TypeId::INT32, 4, 0}
    };
    Schema dest_schema(dest_cols);

    // Build Projection Expression List mapping to old schema coordinates
    std::vector<std::unique_ptr<AbstractExpression>> proj_exprs;
    proj_exprs.push_back(std::make_unique<ColumnValueExpression>(2)); // Map old 'age' (idx 2) to new col 0
    proj_exprs.push_back(std::make_unique<ColumnValueExpression>(0)); // Map old 'id' (idx 0) to new col 1

    // Assemble Execution Tree
    auto scan_exec = std::make_unique<SeqScanExecutor>(&exec_ctx, src_schema, disk.GetTotalPages());
    ProjectionExecutor proj_exec(&exec_ctx, std::move(scan_exec), dest_schema, std::move(proj_exprs));
    proj_exec.Init();

    Tuple t; RID r;
    assert(proj_exec.Next(&t, &r) == true);

    // Verify output tuple matches our updated target projection layout schema offsets
    assert(t.GetInt32(dest_schema, 0) == 27); // 'age' is now located at Column Index 0!
    assert(t.GetInt32(dest_schema, 1) == 99); // 'id' is now located at Column Index 1!

    assert(proj_exec.Next(&t, &r) == false); // End of stream

    std::cout << "Day 7 Projection Engine Pipeline Verification Passed!" << std::endl;
    std::remove(filename.c_str());
}

int main() {
    TestProjectionPipeline();
    return 0;
}