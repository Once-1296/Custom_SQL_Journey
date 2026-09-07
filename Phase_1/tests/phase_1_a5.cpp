#include <iostream>
#include <fstream>
#include <array>
#include <cstdint>
#include <cstring>
#include <cassert>
#include "../src/my_custom_page.hpp" // Ensure your corrected DiskManager/Page are accessible
#include "../src/my_custom_disk_manager.hpp"
void ScanAndPrintUsersOlderThan(DiskManager &disk, uint32_t target_age)
{
    uint32_t total_pages = disk.GetTotalPages();
    std::cout << "🔍 Scanning " << total_pages << " pages for users older than " << target_age << "...\n";

    // YOUR CODE HERE:
    // 1. Loop through every page_id from 0 to total_pages - 1
    for (uint32_t i = 0; i < total_pages; i++)
    {
        // 2. Inside the loop, declare a local stack Page instance
        Page local_page;
        // 3. Call disk.ReadPage(page_id, local_page)
        disk.ReadPage(i, local_page);
        // 4. Get the record count for that page
        uint32_t record_count = local_page.GetRecordCount();
        // 5. Loop through every record index in that page using local_page.GetRecord(i)
        for (uint32_t j = 0; j < record_count; j++)
        {
            UserRecord record = local_page.GetRecord(j);
            // 6. If record.age > target_age, print it to the console!
            if (record.age > target_age)
                std::cout << "User " << record.username << " with Age : " << record.age << std::endl;
        }
    }
}

int main()
{
    std::string filename = "scan_test.db";
    std::remove(filename.c_str());

    {
        DiskManager disk(filename);

        // 1. Setup Phase: Allocate 3 pages
        uint32_t p0_id = disk.AllocatePage();
        uint32_t p1_id = disk.AllocatePage();
        uint32_t p2_id = disk.AllocatePage();

        Page p0, p1, p2;

        // Populate Page 0
        p0.AppendRecord(UserRecord{1, "Alice", 25});
        p0.AppendRecord(UserRecord{2, "Bob", 19});

        // Populate Page 1
        p1.AppendRecord(UserRecord{3, "Charlie", 35});
        p1.AppendRecord(UserRecord{4, "Dan", 15});

        // Populate Page 2
        p2.AppendRecord(UserRecord{5, "Eve", 29});

        // Sync buffers to disk
        disk.WritePage(p0_id, p0);
        disk.WritePage(p1_id, p1);
        disk.WritePage(p2_id, p2);

        // 2. Execution Phase: Run the Table Scan
        // Expected output: Should print Alice (25), Charlie (35), and Eve (29)
        // Should NOT print Bob (19) or Dan (15)
        ScanAndPrintUsersOlderThan(disk, 21);
    }

    std::remove(filename.c_str());
    return 0;
}