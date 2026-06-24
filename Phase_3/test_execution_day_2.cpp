// test_day10.cpp
#include "my_custom_disk_manager.hpp"
#include "my_custom_buffer_pool_manager.hpp"
#include "slotted_page.hpp"
#include "table_heap.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void TestTableHeapChaining() {
    std::string filename = "table_heap_test.bin";
    std::remove(filename.c_str());

    DiskManager disk(filename);
    BufferPoolManager bpm(disk);

    // Set up our initial root page
    uint32_t root_pid = disk.AllocatePage();
    Page* root_page = reinterpret_cast<Page*>(bpm.FetchPage(root_pid));
    root_page->Init();
    root_page->SetNextPageId(0xFFFFFFFF); // End of list marker
    bpm.MarkDirty(root_pid);

    TableHeap heap(&bpm, root_pid);

    // Create a large 2500 byte chunk of data
    std::vector<uint8_t> huge_row(2500, 'A');
    RID rid1, rid2;

    // First insertion must take slot 0 on Page 0
    assert(heap.InsertTuple(huge_row.data(), huge_row.size(), &rid1) == true);
    assert(rid1.page_id == root_pid && rid1.slot_num == 0);
    // std::cout<<"DEBUG: First iteration passed"<<std::endl;
    // Second insertion WILL NOT fit on Page 0 (2500 + 2500 > 4096).
    // This should trigger the TableHeap to automatically allocate Page 1!
    assert(heap.InsertTuple(huge_row.data(), huge_row.size(), &rid2) == true);
    assert(rid2.page_id != root_pid); 
    assert(rid2.slot_num == 0);
    // std::cout<<"DEBUG: Second iteration passed"<<std::endl;

    // Re-fetch root page to ensure link pointer was saved correctly
    Page* verified_root = reinterpret_cast<Page*>(bpm.FetchPage(root_pid));
    assert(verified_root->GetNextPageId() == rid2.page_id);

    std::cout << "Day 10 Table Heap Chaining Verification Passed!" << std::endl;
    std::remove(filename.c_str());
}

int main() {
    TestTableHeapChaining();
    return 0;
}