#ifndef INSERT_EXECUTOR_HPP
#define INSERT_EXECUTOR_HPP

#include "../abstract_executor.hpp" // Assumes your base executor header
#include "insert_plan.hpp"
#include "../catalog.hpp"
#include "../types/tuple.hpp"

class InsertExecutor : public AbstractExecutor
{
private:
    InsertPlanNode *plan_;
    std::unique_ptr<AbstractExecutor> child_executor_;
    Catalog *catalog_;
    TableMetadata *table_meta_{nullptr};
    bool is_executed_{false}; // Tracks whether the summary row has been yielded
    Schema output_schema_;

public:
    InsertExecutor(ExecutorContext *exec_ctx, InsertPlanNode *plan,
                   std::unique_ptr<AbstractExecutor> child_executor,
                   Catalog *catalog)
        : AbstractExecutor(exec_ctx), plan_(plan),
          child_executor_(std::move(child_executor)),
          catalog_(catalog)
    {
        std::vector<Column> cols = {{"insert_count", TypeId::INT32, 4, 0}};
        output_schema_ = Schema(cols);
    }

    void Init() override
    {
        // Initialize the child executor to prepare the incoming tuple stream.
        child_executor_->Init();
        // Query the system catalog using plan_->table_name_ to resolve
        // the destination TableMetadata. Assert or handle cases where the table doesn't exist.
        table_meta_ = catalog_->GetTable(plan_->GetTableName());
        assert(table_meta_ != nullptr);
    }

    bool Next(Tuple *result_tuple, RID *result_rid) override
    {
        // If we already returned the final summary count row, stop the pipeline execution loop.
        if (is_executed_)
        {
            return false;
        }

        uint32_t insert_count = 0;
        Tuple child_tuple;
        RID child_rid;

        // Construct a loop that pulls tuples from the child executor using child_executor_->Next().
        // For each tuple retrieved:
        //  - Extract its raw data pointer and raw data size.
        //  - Call table_meta_->table_heap_->InsertTuple(...) to write the data to disk.
        //  - Increment your tracking insert_count counter.
        while (child_executor_->Next(&child_tuple, &child_rid))
        {
            const uint8_t *data_ptr = child_tuple.GetData();
            const uint32_t size = child_tuple.GetSize();
            RID new_rid;
            table_meta_->table_heap_->InsertTuple(data_ptr, size, &new_rid);
            insert_count++;
        }
        // Construct a brand new single-column summary Tuple containing the insert_count value.
        // Assign this value to the out-parameter *result_tuple.
        // 1. Point directly to the address of your integer variable on the stack
        const uint8_t *summary_bytes = reinterpret_cast<const uint8_t *>(&insert_count);

        // 2. Assign a dummy RID since this runtime summary row isn't saved anywhere on a disk page
        *result_rid = RID(0, 0);

        // 3. Instantiate the Tuple by telling it to copy those 4 bytes
        *result_tuple = Tuple(summary_bytes, sizeof(uint32_t), *result_rid);

        // Flip the tracking switch so the subsequent Next() call correctly breaks the execution loop.
        is_executed_ = true;
        return true;
    }
    const Schema &GetOutputSchema() const override
    {
        return output_schema_;
    }
};

#endif