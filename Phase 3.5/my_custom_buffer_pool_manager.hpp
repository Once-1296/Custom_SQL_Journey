#ifndef MY_CUSTOM_BUFFER_POOL_MANAGER_HPP
#define MY_CUSTOM_BUFFER_POOL_MANAGER_HPP
#include <iostream>
#include <unordered_map>
#include <array>
#include <cstdint>
#include "slotted_page.hpp"           // Your Page class
#include "my_custom_disk_manager.hpp" // Your DiskManager class
#include <list>
#include <set>

class BufferPoolManager
{
private:
    static constexpr size_t POOL_SIZE = 3; // Our RAM can only hold 3 pages at once

    DiskManager &disk_manager;
    std::array<Page, POOL_SIZE> pool; // The physical slots (frames) in RAM
    size_t next_free_frame = 0;       // Tracks where to insert on a cache miss
    // Inside BufferPoolManager:
    // 1. The list stores page_ids. Front is MRU (Most Recently Used), Back is LRU.
    std::list<uint32_t> lru_list;
    std::set<uint32_t> free_frames;
    // 2. The map stores the frame and the iterator (the "bookmark")
    struct PageMetadata
    {
        uint32_t frame_id;
        std::list<uint32_t>::iterator list_iterator;
        bool is_dirty;
    };
    std::unordered_map<uint32_t, PageMetadata> page_table;
    // Now maps page_id to {frame_id, iterator_to_list_node}
public:
    uint32_t cache_hits = 0;
    uint32_t cache_misses = 0;

    BufferPoolManager(DiskManager &disk) : disk_manager(disk)
    {
        for (uint32_t i = 0; i < POOL_SIZE; i++)
        {
            free_frames.insert(i);
        }
    }
    ~BufferPoolManager()
    {
        // Write back all dirty pages to disk upon destruction
        for (const auto &entry : page_table)
        {
            if (entry.second.is_dirty)
            {
                disk_manager.WritePage(entry.first, pool[entry.second.frame_id]);
            }
        }
    }
    void MarkDirty(uint32_t page_id)
    {
        if (page_table.find(page_id) != page_table.end())
        {
            page_table[page_id].is_dirty = true;
        }
    }
    Page *FetchPage(uint32_t page_id)
    {
        // --- HIT CASE ---
        auto it = page_table.find(page_id);
        if (it != page_table.end())
        {
            cache_hits++;
            // Splice moves the existing node to the front of the list
            lru_list.splice(lru_list.begin(), lru_list, it->second.list_iterator);
            return &pool[it->second.frame_id];
        }

        // --- MISS CASE ---
        cache_misses++;
        uint32_t frame_to_use;

        if (lru_list.size() >= POOL_SIZE)
        {
            // 1. Evict LRU victim
            uint32_t victim_id = lru_list.back();
            free_frames.insert(page_table[victim_id].frame_id); // Reuse the victim's frame
            if (page_table[victim_id].is_dirty)
            {
                disk_manager.WritePage(victim_id, pool[page_table[victim_id].frame_id]);
            }
            page_table.erase(victim_id);
            lru_list.pop_back();
        }
        // 2. Use the smallest available frame
        frame_to_use = *free_frames.begin();

        // 3. Load from disk directly into the target pool frame
        disk_manager.ReadPage(page_id, pool[frame_to_use]);
        // 4. Update metadata
        lru_list.push_front(page_id);
        page_table[page_id] = {frame_to_use, lru_list.begin(), false};
        free_frames.erase(frame_to_use);

        return &pool[frame_to_use];
    }

    uint32_t NewPage() const
    {
        return disk_manager.AllocatePage();
    }

    uint32_t DeletePageHelper(uint32_t page_id)
    {
        uint32_t deleted_count = 0;
        while (page_id != 0xFFFFFFFF)
        {
            // Remove from buffer pool if present
            Page *page = FetchPage(page_id);
            if(page == nullptr)break;
            auto it = page_table.find(page_id);
            if (it != page_table.end())
            {
                free_frames.insert(it->second.frame_id);
                lru_list.erase(it->second.list_iterator);
                page_table.erase(it);
            }
            page_id = page->GetNextPageId();
            // Delete from disk and all successors
            disk_manager.DeletePage(page_id);
            deleted_count++;
        }
        return deleted_count;
    }
};
#endif