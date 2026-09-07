// test_day13.cpp
#include "../src/storage/my_custom_disk_manager.hpp"
#include "../src/storage/my_custom_buffer_pool_manager.hpp"
#include "../src/catalog.hpp"
#include "../src/executors/insert_executor.hpp"
#include <cassert>
#include <iostream>
#include <vector>

// Minimal Mock Executor to stream test rows upward
class MockChildExecutor : public AbstractExecutor {
private:
    std::vector<std::string> mock_data_ = {"AliceRecordData", "BobRecordData"};
    size_t cursor_ = 0;
    Schema mock_schema =  Schema({{"word", TypeId::VARCHAR, 32, 0}});
public:
    void Init() override { cursor_ = 0; }
    bool Next(Tuple *tuple, RID *rid) override {
        if (cursor_ >= mock_data_.size()) return false;
        
        // Emulate raw byte data payload formatting
        const std::string &target = mock_data_[cursor_];
        *rid = RID(0, static_cast<uint32_t>(cursor_));
        *tuple = Tuple(reinterpret_cast<const uint8_t*>(target.data()), target.size(),*rid);
        cursor_++;
        return true;
    }
    const Schema &GetOutputSchema() const override
    {
        return mock_schema;
    }
};

void TestInsertPipeline() {
    std::string filename = "insert_test.bin";
    std::remove(filename.c_str());

    {
        DiskManager disk(filename);
        BufferPoolManager bpm(disk);
        Catalog catalog(&bpm);
        ExecutorContext exec_ctx(bpm);
        // Define schema and register table
        std::vector<Column> cols = {{"payload", TypeId::VARCHAR, 32, 0}};
        Schema schema(cols);
        assert(catalog.CreateTable("users", schema) == true);

        // Instantiate our write execution tree
        InsertPlanNode plan("users");
        auto mock_child = std::make_unique<MockChildExecutor>();
        InsertExecutor insert_exec(&exec_ctx,&plan, std::move(mock_child), &catalog);

        insert_exec.Init();

        Tuple summary_tuple;
        RID dummy_rid;
        // First execution call must succeed and yield our summary count
        assert(insert_exec.Next(&summary_tuple, &dummy_rid) == true);

        // Subsequent execution calls must yield false indicating EOF
        assert(insert_exec.Next(&summary_tuple, &dummy_rid) == false);
    } 

    // The BufferPoolManager destructor flushes the data to the hard drive upon exiting the scope above.
    // Let's re-open the physical file and verify the data was saved sequentially.
    {
        DiskManager disk(filename);
        BufferPoolManager bpm(disk);
        
        // Fetch page 0 (which was assigned as our table's root page frame)
        Page* page = bpm.FetchPage(0);
        assert(page->GetSlotCount() == 2); // Both Alice and Bob must be sitting on disk!
        
        uint32_t size1, size2;
        const uint8_t* ptr1 = page->GetTuplePtr(0, &size1);
        const uint8_t* ptr2 = page->GetTuplePtr(1, &size2);

        assert(std::string(reinterpret_cast<const char*>(ptr1), size1) == "AliceRecordData");
        assert(std::string(reinterpret_cast<const char*>(ptr2), size2) == "BobRecordData");
    }

    std::cout << "Day 13 Write Execution Engine Pipeline Verified!" << std::endl;
    std::remove(filename.c_str());
}

int main() {
    TestInsertPipeline();
    return 0;
}