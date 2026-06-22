#include <iostream>
#include <unordered_map>
#include <array>
#include <cstdint>
#include <cassert>
#include "my_custom_buffer_pool_manager.hpp"

int main()
{
    std::string filename = "integration_test.db";
    std::remove(filename.c_str());

    // 1. Setup: Load a page
    {
        DiskManager disk(filename);
        uint32_t p0 = disk.AllocatePage();
        Page p0_data;
        p0_data.AppendRecord(UserRecord{1, "InitialData", 20});
        disk.WritePage(p0, p0_data);
    }

    // 2. Perform operations with Dirty Bit
    {
        DiskManager disk(filename);
        BufferPoolManager bpm(disk);

        // Fetch
        Page *p = bpm.FetchPage(0);

        // Modify the page in RAM
        UserRecord updated{1, "UpdatedData", 99};
        p->AppendRecord(updated); // Assume you added an Overwrite/Update method to Page
        // CRITICAL: Mark as dirty!
        bpm.MarkDirty(0);

        // Trigger eviction by filling the pool with other pages
        bpm.FetchPage(1);
        bpm.FetchPage(2);
        bpm.FetchPage(3); // Page 0 should now be evicted and written to disk
    }

    // 3. Verify Persistence
    {
        DiskManager disk(filename);
        Page p0_final;
        disk.ReadPage(0, p0_final);

        // If the dirty bit worked, this will be 99, not 20!
        assert(p0_final.GetRecord(1).age == 99);
        std::cout << "✅ Dirty bit persistence test passed!" << std::endl;
    }

    std::remove(filename.c_str());
    return 0;
}