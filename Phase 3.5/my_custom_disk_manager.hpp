#ifndef MY_CUSTOM_DISK_MANAGER_HPP
#define MY_CUSTOM_DISK_MANAGER_HPP

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include "slotted_page.hpp"

class DiskManager {
private:
    std::fstream db_file;
    std::string filename;
    uint32_t num_pages_; // Cached page counter

public:
    DiskManager(const std::string &db_filename) : filename(db_filename), num_pages_(0) {
        db_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!db_file.is_open()) {
            db_file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
            db_file.close();
            db_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        }

        // Initialize our cached count ONCE at startup
        db_file.clear();
        db_file.seekg(0, std::ios::end);
        size_t file_size = static_cast<size_t>(db_file.tellg());
        num_pages_ = file_size / 4096;
    }

    ~DiskManager() {
        if (db_file.is_open())
            db_file.close();
    }

    void WritePage(uint32_t page_id, const Page &page) {
        db_file.clear();
        size_t offset = static_cast<size_t>(page_id) * 4096;
        db_file.seekp(offset);
        db_file.write(reinterpret_cast<const char *>(page.ReadRawData()), 4096);
        
        if (db_file.fail()) {
            std::cerr << "CRITICAL ERROR: Failed to write Page " << page_id << " to disk!" << std::endl;
        }
        db_file.flush();
    }

    void ReadPage(uint32_t page_id, Page &out_page) { 
        db_file.clear();
        size_t offset = static_cast<size_t>(page_id) * 4096;
        db_file.seekg(offset);
        db_file.read(reinterpret_cast<char *>(out_page.WriteRawData()), 4096);
    }

    // Instantly returns cached state without hitting disk metadata
    uint32_t GetTotalPages() const {
        return num_pages_;
    }

    uint32_t AllocatePage() {
        db_file.clear();
        uint32_t allocated_id = 0; // Find Dead Page or make new Page
        while(allocated_id < num_pages_)
        {
            Page page;
            ReadPage(allocated_id, page);
            bool isAlive = page.isAlive();
            if(!isAlive)
            {
                Page blank_page;
                WritePage(allocated_id, blank_page);
                return allocated_id;
            }
            allocated_id++;
        }
        Page blank_page;
        WritePage(allocated_id, blank_page);
        num_pages_++; // Safely increment cache
        return allocated_id;
    }

    bool DeletePage(uint32_t page_id) {
        db_file.clear();
        if (page_id!= 0xFFFFFFFF) {
            Page page;
            ReadPage(page_id, page);
            page.DeletePage();
            WritePage(page_id, page);
            return true;
        }
        return false;
    }
};

#endif