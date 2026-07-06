#ifndef INSERTION_EXECUTOR_HPP
#define INERTION_EXECUTOR_HPP

#include "abstract_executor.hpp"

class InsertionExecutor : public AbstractExecutor
{
private:
    Schema schema_;

    // Iteration state
    uint32_t current_page_id_;
    uint32_t previous_page_id_;
    uint32_t pages_scanned_;
    uint32_t start_page_id_;

public:
    InsertionExecutor(ExecutorContext *exec_ctx, const Schema &schema, uint32_t start_page_id = 0, uint32_t previous_page_id = 0xFFFFFFFF)
        : AbstractExecutor(exec_ctx), schema_(schema), current_page_id_(start_page_id), start_page_id_(start_page_id), pages_scanned_(0), previous_page_id_(previous_page_id) {}

    void Init() override
    {
        // Reset execution cursors to point to the very first record slot
        current_page_id_ = start_page_id_;
        previous_page_id_ = 0xFFFFFFF;
        pages_scanned_ = 0;
    }

    bool Next(Tuple *tuple, RID *rid) override
    {
        BufferPoolManager *bpm = exec_ctx_->GetBufferPoolManager();
        uint32_t tuple_size = schema_.GetTupleSize();
        {
            Page pg;
            if (pg.maxPageSpace() < tuple_size)
                return false;
        }
        while (current_page_id_ != 0xFFFFFFFF)
        {
            Page *page = bpm->FetchPage(current_page_id_);
            if (page == nullptr)
            {
                return false;
            }
            uint32_t out_slot_num;
            if (page->InsertTuple(tuple->GetData(), tuple_size, &out_slot_num))
            {
                bpm->MarkDirty(current_page_id_);
                *rid = RID(current_page_id_, out_slot_num);
                return true;
            }
            // Current page exhausted, advance to the next block
            previous_page_id_ = current_page_id_;
            current_page_id_ = page->GetNextPageId();
            pages_scanned_++;
        }
        if (previous_page_id_ != 0xFFFFFFFF)
        {
            Page *page = bpm->FetchPage(previous_page_id_);
            if (page == nullptr)
                return false;
            current_page_id_ = bpm->NewPage();
            page->SetNextPageId(current_page_id_);
            page = bpm->FetchPage(current_page_id_);
            uint32_t out_slot_num;
            page->InsertTuple(tuple->GetData(), tuple_size, &out_slot_num);
            bpm->MarkDirty(current_page_id_);
            *rid = RID(current_page_id_, out_slot_num);
            return true;
        }
        return false; // what reached
    }

    const Schema &GetOutputSchema() const override
    {
        return schema_;
    }
};

#endif