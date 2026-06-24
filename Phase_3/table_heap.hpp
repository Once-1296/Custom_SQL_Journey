// table_heap.hpp
#ifndef TABLE_HEAP_HPP
#define TABLE_HEAP_HPP

#include "my_custom_buffer_pool_manager.hpp" // Assumes it manages SlottedPage casts or raw frames
#include "slotted_page.hpp"
#include "tuple.hpp"

class TableHeap
{
private:
    BufferPoolManager *bpm_;
    uint32_t first_page_id_;

public:
    TableHeap(BufferPoolManager *bpm, uint32_t first_page_id)
        : bpm_(bpm), first_page_id_(first_page_id)
    {

        // GUARD RAIL: If the table is brand new, allocate its root page immediately
        if (first_page_id_ == 0xFFFFFFFF)
        {
            first_page_id_ = bpm_->NewPage();
            Page *root_page = bpm_->FetchPage(first_page_id_);
            root_page->Init();
            bpm_->MarkDirty(first_page_id_);
        }
    }

    uint32_t GetFirstPageId() const { return first_page_id_; }

    // Inserts a tuple into the table, automatically creating pages if necessary
    bool InsertTuple(const uint8_t *tuple_data, uint32_t size, RID *out_rid)
    {
        uint32_t current_page_id = first_page_id_;

        while (true)
        {
            Page *cur_page = bpm_->FetchPage(current_page_id);
            if (cur_page == nullptr)
                return false;

            uint32_t slot_num = 0;
            if (cur_page->InsertTuple(tuple_data, size, &slot_num))
            {
                out_rid->slot_num = slot_num;
                out_rid->page_id = current_page_id;
                bpm_->MarkDirty(current_page_id); // Safely mark dirty
                return true;
            }

            uint32_t next_pid = cur_page->GetNextPageId();
            if (next_pid != 0xFFFFFFFF && next_pid != 0)
            {
                current_page_id = next_pid;
                continue;
            }

            // Space exhausted. Safe Allocation Strategy:
            // 1. Allocate the new page token
            uint32_t new_page_id = bpm_->NewPage();

            // 2. Link the current page BEFORE fetching the new one to avoid eviction hazards
            cur_page->SetNextPageId(new_page_id);
            bpm_->MarkDirty(current_page_id); // CRITICAL: Save the link!

            // 3. Now initialize the new page safely
            Page *new_page = bpm_->FetchPage(new_page_id);
            new_page->Init();
            bpm_->MarkDirty(new_page_id);

            // 4. Advance our tracking ID and let the loop naturally re-fetch
            current_page_id = new_page_id;
        }
    }
    
};

#endif