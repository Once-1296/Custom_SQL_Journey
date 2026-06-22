#ifndef MY_CUSTOM_BUFFER_POOL_MANAGER_HPP
#define MY_CUSTOM_BUFFER_POOL_MANAGER_HPP
#include <iostream>
#include <unordered_map>
#include <array>
#include <cstdint>
#include "my_custom_page.hpp"         // Your Page class
#include "my_custom_disk_manager.hpp" // Your DiskManager class
#include <list>

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

    BufferPoolManager(DiskManager &disk) : disk_manager(disk) {}

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
            frame_to_use = page_table[victim_id].frame_id; // Reuse the victim's frame
            if (page_table[victim_id].is_dirty)
            {
                disk_manager.WritePage(victim_id, pool[page_table[victim_id].frame_id]);
            }
            page_table.erase(victim_id);
            lru_list.pop_back();
        }
        else
        {
            // 2. Use the next available frame
            frame_to_use = next_free_frame++;
        }

        // 3. Load from disk directly into the target pool frame
        disk_manager.ReadPage(page_id, pool[frame_to_use]);

        // 4. Update metadata
        lru_list.push_front(page_id);
        page_table[page_id] = {frame_to_use, lru_list.begin(), false};

        return &pool[frame_to_use];
    }
};
#endif