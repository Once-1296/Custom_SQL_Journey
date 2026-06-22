#include <iostream>
#include <fstream>
#include <array>
#include <cstdint>
#include <cassert>

// ... Include your Page class from yesterday here ...
#include "my_custom_page.hpp"

class DiskManager
{
private:
    std::fstream db_file;
    std::string filename;

public:
    DiskManager(const std::string &db_filename) : filename(db_filename)
    {
        db_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!db_file.is_open())
        {
            db_file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
            db_file.close();
            db_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    ~DiskManager()
    {
        if (db_file.is_open())
            db_file.close();
    }

    // Re-use your fixed WritePage and ReadPage methods here...
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
    { // 1. Calculate file offset
        size_t offset = static_cast<size_t>(page_id) * 4096;

        // 2. Move file read pointer to the offset
        db_file.seekg(offset);

        // 3. Read 4KB directly into the destination page's internal buffer
        db_file.read(reinterpret_cast<char *>(out_page.WriteRawData()), 4096);
    }

    // Returns the total number of 4KB blocks currently inside the file
    uint32_t GetTotalPages()
    {
        // Clear any error flags (like EOF) that stream might have hit previously
        db_file.clear();

        // YOUR CODE HERE:
        // 1. Seek to the absolute end of the file using seekg
        db_file.seekg(0,std::ios::end);
        // 2. Use tellg() to get the file size in bytes
        size_t file_size = static_cast<size_t>(db_file.tellg());
        // 3. Return (file_size / 4096)
        return file_size / 4096;
    }

    // Grows the file by one blank page and returns its new Page ID
    uint32_t AllocatePage()
    {
        db_file.clear();

        // YOUR CODE HERE:
        // 1. Find out how many pages we have right now (this will be our new page's ID!)
        uint32_t num_pages = GetTotalPages();
        // 2. Create a temporary, blank 4096-byte array or Page object initialized to zeros
        Page new_page;
        // 3. Write this blank page to the very end of the file using your WritePage method
        WritePage(num_pages, new_page);
        // 4. Return the new page_id
        return num_pages;
    }
};

int main()
{
    std::string filename = "test_grow.db";
    std::remove(filename.c_str());

    {
        DiskManager disk(filename);
        assert(disk.GetTotalPages() == 0); // Fresh database file should be empty

        // Allocate 3 pages sequentially
        uint32_t p0 = disk.AllocatePage();
        uint32_t p1 = disk.AllocatePage();
        uint32_t p2 = disk.AllocatePage();

        assert(p0 == 0);
        assert(p1 == 1);
        assert(p2 == 2);
        assert(disk.GetTotalPages() == 3);

        // Now that the file is physically 3 pages large, writing to them is completely safe!
        Page write_p1;
        // ... (add some sample records to write_p1 if you want) ...
        disk.WritePage(1, write_p1);
    }

    std::remove(filename.c_str());
    std::cout << "📈 Day 4 Challenge Passed! Your engine can scale its physical storage safely." << std::endl;
    return 0;
}