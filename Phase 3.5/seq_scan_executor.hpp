#ifndef SEQ_SCAN_EXECUTOR_HPP
#define SEQ_SCAN_EXECUTOR_HPP

#include "abstract_executor.hpp"

class SeqScanExecutor : public AbstractExecutor {
private:
    Schema schema_;
    uint32_t total_pages_;
    
    // Iteration state
    uint32_t current_page_id_;
    uint32_t current_slot_num_;
    uint32_t pages_scanned_;
    uint32_t start_page_id_;


public:
    SeqScanExecutor(ExecutorContext *exec_ctx, const Schema &schema, uint32_t total_pages, uint32_t start_page_id = 0)
        : AbstractExecutor(exec_ctx), schema_(schema), total_pages_(total_pages),
          current_page_id_(start_page_id), current_slot_num_(0), start_page_id_(start_page_id), pages_scanned_(0) {}

    void Init() override {
        // Reset execution cursors to point to the very first record slot
        current_page_id_ = start_page_id_;
        current_slot_num_ = 0;
        pages_scanned_ = 0;
    }

    bool Next(Tuple *tuple, RID *rid) override {
        BufferPoolManager* bpm = exec_ctx_->GetBufferPoolManager();
        uint32_t tuple_size = schema_.GetTupleSize();


        while (current_page_id_ != 0xFFFFFFFF) {
            Page* page = bpm->FetchPage(current_page_id_);
            if (page == nullptr) {
                return false;
            }

            uint32_t record_count = page->GetSlotCount();

            if (current_slot_num_ < record_count) {
                // Retrieve the direct zero-copy pointer to the tuple data 
                // from the generic page using our schema's runtime 'tuple_size'.
                uint32_t out_tuple_size;
                const uint8_t* tuple_ptr = page->GetTuplePtr(current_slot_num_, &out_tuple_size);
                assert(out_tuple_size==tuple_size);
                assert(tuple_ptr != nullptr);
                // Construct the zero-copy Tuple instance and populate the output pointers
                *tuple = Tuple(tuple_ptr, tuple_size, RID(current_page_id_, current_slot_num_));
                *rid = tuple->GetRID();
                // Advance slot pointer for the subsequent Volcano step
                current_slot_num_++;
                return true;
            }

            // Current page exhausted, advance to the next block
            current_page_id_= page->GetNextPageId();
            pages_scanned_++;
            current_slot_num_ = 0;
        }

        return false; // EOF reached
    }

    const Schema& GetOutputSchema() const override {
        return schema_;
    }
};

#endif