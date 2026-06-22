#include <iostream>
#include <unordered_map>
#include <array>
#include <cstdint>
#include <cassert>
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
    };
    std::unordered_map<uint32_t, PageMetadata> page_table;
    // Now maps page_id to {frame_id, iterator_to_list_node}
public:
    uint32_t cache_hits = 0;
    uint32_t cache_misses = 0;

    BufferPoolManager(DiskManager &disk) : disk_manager(disk) {}

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
        page_table[page_id] = {frame_to_use, lru_list.begin()};

        return &pool[frame_to_use];
    }
};

int main()
{
    std::string filename = "bpm_test.db";
    std::remove(filename.c_str());

    // 1. Setup raw disk data
    {
        DiskManager disk(filename);
        uint32_t p0 = disk.AllocatePage();
        uint32_t p1 = disk.AllocatePage();

        Page page0;
        page0.AppendRecord(UserRecord{42, "CachedUser", 30});
        disk.WritePage(p0, page0);
    }

    // 2. Test the Buffer Pool
    {
        DiskManager disk(filename);
        BufferPoolManager bpm(disk);

        // First fetch: must be a cache miss (reads disk)
        Page *fetch1 = bpm.FetchPage(0);
        assert(fetch1 != nullptr);
        assert(fetch1->GetRecord(0).id == 42);
        assert(bpm.cache_misses == 1);
        assert(bpm.cache_hits == 0);

        // Second fetch of same page: MUST be a cache hit (pure RAM!)
        Page *fetch2 = bpm.FetchPage(0);
        assert(fetch2 != nullptr);
        assert(fetch2->GetRecord(0).id == 42);
        assert(bpm.cache_misses == 1);
        assert(bpm.cache_hits == 1); // Hit counted!

        // Confirm they point to the exact same memory address space in RAM
        assert(fetch1 == fetch2);
    }

    std::remove(filename.c_str());
    std::cout << "🧠 Day 6 Challenge Passed! Your database engine now has an active RAM cache memory pool." << std::endl;
    return 0;
}