#ifndef MY_CUSTOM_DISK_MANAGER_HPP
#define MY_CUSTOM_DISK_MANAGER_HPP

#include <iostream>
#include <fstream>
#include <array>
#include <cstdint>
#include <cassert>

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
        // 1. CLEAR the error flags before doing any IO
        db_file.clear();

        // 2. Calculate file offset
        size_t offset = static_cast<size_t>(page_id) * 4096;

        // 3. Move file write pointer
        db_file.seekp(offset);

        // 4. Perform the write
        db_file.write(reinterpret_cast<const char *>(page.ReadRawData()), 4096);

        // 5. CRITICAL: Check if the write actually succeeded!
        if (db_file.fail())
        {
            std::cerr << "CRITICAL ERROR: Failed to write Page " << page_id << " to disk!" << std::endl;
        }

        // 6. Flush to disk
        db_file.flush();
    }
    void ReadPage(uint32_t page_id, Page &out_page)
    { 
        db_file.clear();
        // 1. Calculate file offset
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
        db_file.seekg(0, std::ios::end);
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
#endif