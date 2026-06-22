#include <iostream>
#include <fstream>
#include <array>
#include <cstring>
#include <cstdint>
#include <cassert>
#include "my_custom_page.hpp"
// Reuse your UserRecord and Page class here.

class DiskManager
{
private:
    std::fstream db_file;
    std::string filename;

public:
    DiskManager(const std::string &db_filename) : filename(db_filename)
    {
        // Open file for reading, writing, in binary mode. Create it if it doesn't exist.
        db_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!db_file.is_open())
        {
            // If file doesn't exist, create it by truncating
            db_file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
            db_file.close();
            // Reopen with read/write access
            db_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        }
        assert(db_file.is_open() && "Failed to open database file!");
    }

    ~DiskManager()
    {
        if (db_file.is_open())
        {
            db_file.close();
        }
    }

    void WritePage(uint32_t page_id, const Page &page)
    {
        // 1. Calculate file offset
        size_t offset = static_cast<size_t>(page_id) * 4096;

        // 2. Move file write pointer to the offset
        db_file.seekp(offset);

        // 3. Cast the uint8_t* directly to const char* and write 4KB
        db_file.write(reinterpret_cast<const char *>(page.ReadRawData()), 4096);

        // 4. Flush to force the OS to write to disk right now (essential for durability)
        db_file.flush();
    }

    void ReadPage(uint32_t page_id, Page &out_page)
    {
        // 1. Calculate file offset
        size_t offset = static_cast<size_t>(page_id) * 4096;

        // 2. Move file read pointer to the offset
        db_file.seekg(offset);

        // 3. Read 4KB directly into the destination page's internal buffer
        db_file.read(reinterpret_cast<char *>(out_page.WriteRawData()), 4096);
    }
};

int main()
{
    std::string filename = "test_engine.db";

    // Remove old test file if it exists
    std::remove(filename.c_str());

    {
        DiskManager disk_manager(filename);

        // Create page 0 and add data
        Page p0;
        UserRecord u1{101, "Alice", 22};
        p0.AppendRecord(u1);

        // Create page 1 and add data
        Page p1;
        UserRecord u2{202, "Bob", 25};
        p1.AppendRecord(u2);

        // Write pages out-of-order to test arbitrary seeking
        disk_manager.WritePage(1, p1);
        disk_manager.WritePage(0, p0);
    } // DiskManager closes file here

    {
        // Reopen file to simulate database rebooting from scratch
        DiskManager disk_manager(filename);

        Page fetched_p0;
        Page fetched_p1;

        disk_manager.ReadPage(0, fetched_p0);
        disk_manager.ReadPage(1, fetched_p1);

        // Verify data survived the trip to the hard drive
        assert(fetched_p0.GetRecordCount() == 1);
        assert(fetched_p0.GetRecord(0).id == 101);

        assert(fetched_p1.GetRecordCount() == 1);
        assert(fetched_p1.GetRecord(0).id == 202);
    }

    std::remove(filename.c_str()); // Clean up file
    std::cout << "💾 Day 3 Challenge Passed! Your engine can now persist pages to a physical disk file." << std::endl;
    return 0;
}