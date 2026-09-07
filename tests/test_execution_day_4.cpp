// test_day4.cpp
#include "../src/storage/my_custom_disk_manager.hpp"
#include "../src/storage/my_custom_buffer_pool_manager.hpp"
#include "../src/executor_context.hpp"
#include "../src/executors/seq_scan_executor.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <array>
#include <cstring>

void TestGenericSeqScan() {
    std::string filename = "test_generic_execution_db.bin";
    std::remove(filename.c_str()); // Clean state

    DiskManager disk(filename);
    BufferPoolManager bpm(disk);
    ExecutorContext exec_ctx(bpm);

    // Define standard 40-byte User Schema
    std::vector<Column> cols = {
        {"id",       TypeId::INT32,   4, 0},
        {"username", TypeId::VARCHAR, 32, 0},
        {"age",      TypeId::INT32,   4, 0}
    };
    Schema schema(cols);
    uint32_t tuple_size = schema.GetTupleSize();

    // Allocate two raw storage blocks
    uint32_t p0_id = disk.AllocatePage();
    uint32_t p1_id = disk.AllocatePage();

    // Manual data serialization matching our schema layouts
    std::array<uint8_t, 40> buffer_alice{0};
    int32_t id_1 = 10; int32_t age_1 = 25;
    std::memcpy(buffer_alice.data() + 0, &id_1, 4);
    std::memcpy(buffer_alice.data() + 4, "alice", 5);
    std::memcpy(buffer_alice.data() + 36, &age_1, 4);

    std::array<uint8_t, 40> buffer_bob{0};
    int32_t id_2 = 20; int32_t age_2 = 30;
    std::memcpy(buffer_bob.data() + 0, &id_2, 4);
    std::memcpy(buffer_bob.data() + 4, "bob", 3);
    std::memcpy(buffer_bob.data() + 36, &age_2, 4);

    // Populate Page 0
    Page* p0 = bpm.FetchPage(p0_id);
    p0->AppendRecord(buffer_alice.data(), tuple_size);
    p0->AppendRecord(buffer_bob.data(), tuple_size);
    bpm.MarkDirty(p0_id);

    // Populate Page 1
    std::array<uint8_t, 40> buffer_charlie{0};
    int32_t id_3 = 30; int32_t age_3 = 35;
    std::memcpy(buffer_charlie.data() + 0, &id_3, 4);
    std::memcpy(buffer_charlie.data() + 4, "charlie", 7);
    std::memcpy(buffer_charlie.data() + 36, &age_3, 4);

    Page* p1 = bpm.FetchPage(p1_id);
    p1->AppendRecord(buffer_charlie.data(), tuple_size);
    bpm.MarkDirty(p1_id);

    // Instantiate and execute our new SeqScanExecutor
    SeqScanExecutor scan_exec(&exec_ctx, schema, disk.GetTotalPages());
    scan_exec.Init();

    Tuple t; RID r;

    // Tuple 1 Verification
    assert(scan_exec.Next(&t, &r) == true);
    assert(t.GetInt32(schema, 0) == 10);
    assert(t.GetVarchar(schema, 1) == "alice");
    assert(r.page_id == 0 && r.slot_num == 0);

    // Tuple 2 Verification
    assert(scan_exec.Next(&t, &r) == true);
    assert(t.GetInt32(schema, 0) == 20);
    assert(t.GetVarchar(schema, 1) == "bob");
    assert(r.page_id == 0 && r.slot_num == 1);

    // Tuple 3 Verification (Page Boundary crossing test)
    assert(scan_exec.Next(&t, &r) == true);
    assert(t.GetInt32(schema, 0) == 30);
    assert(t.GetVarchar(schema, 1) == "charlie");
    assert(r.page_id == 1 && r.slot_num == 0);

    // End of scan confirmation
    assert(scan_exec.Next(&t, &r) == false);

    std::cout << "Day 4 Schema-Agnostic Physical SeqScan Tests Passed!" << std::endl;
    std::remove(filename.c_str());
}

int main() {
    TestGenericSeqScan();
    return 0;
}