#ifndef MOCK_EXECUTOR_HPP
#define MOCK_EXECUTOR_HPP

#include "abstract_executor.hpp"

class MockExecutor : public AbstractExecutor {
private:
    Schema schema_;
    uint32_t limit_;
    uint32_t cursor_;
    std::vector<uint8_t> dummy_buffer_; // Holds our generated mock tuple data

public:
    MockExecutor(ExecutorContext *exec_ctx, const Schema &schema, uint32_t limit)
        : AbstractExecutor(exec_ctx), schema_(schema), limit_(limit), cursor_(0) {
        dummy_buffer_.resize(schema_.GetTupleSize());
    }

    void Init() override {
        // TODO 1: Reset the iteration state so Next() starts from the beginning again
        cursor_ = 0;
    }

    bool Next(Tuple *tuple, RID *rid) override {
        if (cursor_ >= limit_) {
            return false;
        }

        // Write the current cursor value into the start of our dummy buffer as our Mock ID
        std::memcpy(dummy_buffer_.data(), &cursor_, sizeof(cursor_));

        // TODO 3: Assign a zero-copy Tuple instance to the output pointer 'tuple'
        // Hint: Use the constructor: Tuple(const uint8_t* data_ptr, uint32_t size, RID rid)
        // For the RID, pass a mock identifier like RID(0, cursor_)
        // BufferPoolManager *bpm = exec_ctx_->GetBufferPoolManager();
        // Page* page = bpm->FetchPage(0);
        // const uint8_t *data_ptr = page->ReadRawData();
        uint32_t tuple_size = schema_.GetTupleSize();
        *tuple = Tuple(dummy_buffer_.data(),tuple_size,RID(0,cursor_));
        *rid = tuple->GetRID();

        // TODO 4: Advance your cursor state
        cursor_++;
        return true;
    }

    const Schema& GetOutputSchema() const override {
        return schema_;
    }
};

#endif
