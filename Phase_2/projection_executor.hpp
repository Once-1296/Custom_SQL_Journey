#ifndef PROJECTION_EXECUTOR_HPP
#define PROJECTION_EXECUTOR_HPP

#include "abstract_executor.hpp"
#include "abstract_expression.hpp"
#include <vector>

class ProjectionExecutor : public AbstractExecutor
{
private:
    std::unique_ptr<AbstractExecutor> child_executor_;
    Schema output_schema_;
    std::vector<std::unique_ptr<AbstractExpression>> expressions_;

    // Local workspace buffer to hold the newly materialized tuple data
    std::vector<uint8_t> tuple_buffer_;

public:
    ProjectionExecutor(ExecutorContext *exec_ctx,
                       std::unique_ptr<AbstractExecutor> child_executor,
                       Schema output_schema,
                       std::vector<std::unique_ptr<AbstractExpression>> expressions)
        : AbstractExecutor(exec_ctx),
          child_executor_(std::move(child_executor)),
          output_schema_(output_schema),
          expressions_(std::move(expressions))
    {
        // Resize our local buffer to match the exact byte size of our target output layout
        tuple_buffer_.resize(output_schema_.GetTupleSize());
    }

    void Init() override
    {
        child_executor_->Init();
    }

    bool Next(Tuple *tuple, RID *rid) override
    {
        Tuple child_tuple;
        RID child_rid;

        // Pull the next available tuple from the child operator
        if (!child_executor_->Next(&child_tuple, &child_rid))
        {
            return false; // Child hit EOF
        }

        // Clear local buffer to prevent data bleeding across evaluations
        std::fill(tuple_buffer_.begin(), tuple_buffer_.end(), 0);

        // Re-shape the data layout matching output_schema_
        // Iterate through all projection expressions using an index 'i'
        for (size_t i = 0; i < expressions_.size(); ++i)
        {
            Value val = expressions_[i]->Evaluate(&child_tuple, child_executor_->GetOutputSchema());
            const Column &target_col = output_schema_.GetColumn(i);

            if (target_col.type == TypeId::INT32)
            {
                int32_t int_val = val.AsInt32();
                // Copy from the address of our local stack variable directly into the schema offset
                std::memcpy(tuple_buffer_.data() + target_col.offset, &int_val, target_col.length);
            }
            else if (target_col.type == TypeId::VARCHAR)
            {
                std::string str_val = val.AsVarchar();
                // Safely prevent overruns if the string somehow exceeds the fixed column limits
                size_t copy_len = std::min(str_val.length(), static_cast<size_t>(target_col.length));
                
                // Use .data() to safely access the char array pointer of the string
                std::memcpy(tuple_buffer_.data() + target_col.offset, str_val.data(), copy_len);
            }
            else
            {
                assert(false && "Unknown Type Encountered");
            }
        }

        // Wrap the newly filled tuple_buffer_ using the total schema layout size
        *tuple = Tuple(tuple_buffer_.data(), output_schema_.GetTupleSize(), child_tuple.GetRID());
        *rid = child_tuple.GetRID();

        return true;
    }

    const Schema &GetOutputSchema() const override
    {
        return output_schema_;
    }
};

#endif