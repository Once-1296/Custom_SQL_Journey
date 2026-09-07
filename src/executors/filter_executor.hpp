#ifndef FILTER_EXECUTOR_HPP
#define FILTER_EXECUTOR_HPP

#include "../abstract_executor.hpp"
#include "../abstract_expression.hpp"

class FilterExecutor : public AbstractExecutor
{
private:
    std::unique_ptr<AbstractExecutor> child_executor_;
    std::unique_ptr<AbstractExpression> predicate_;

public:
    FilterExecutor(ExecutorContext *exec_ctx,
                   std::unique_ptr<AbstractExecutor> child_executor,
                   std::unique_ptr<AbstractExpression> predicate)
        : AbstractExecutor(exec_ctx),
          child_executor_(std::move(child_executor)),
          predicate_(std::move(predicate)) {}

    void Init() override
    {
        // Initialize the child executor so it resets its internal page/slot cursors
        child_executor_->Init();
    }

    bool Next(Tuple *tuple, RID *rid) override
    {
        // Implement the Volcano filter loop
        // 1. Keep calling child_executor_->Next(tuple, rid) in a loop until it returns false (EOF).

        while (child_executor_->Next(tuple, rid))
        {
            // 2. For every tuple produced by the child, evaluate it using predicate_->Evaluate(tuple, GetOutputSchema()).
            Value expr_value = predicate_ -> Evaluate(tuple, GetOutputSchema());
            assert(expr_value.GetType() == TypeId::INT32);
            // 3. Remember our truth convention: an INT32 value of 1 means True.
            int32_t result = expr_value.AsInt32();
            // 4. If the predicate evaluates to true, return true immediately (leaving 'tuple' and 'rid' populated).
            if(result == 1){
                return true;
            }
            // 5. If the child hits EOF without any tuple passing the predicate, return false.
        }
        return false;
    }

    const Schema &GetOutputSchema() const override
    {
        // A filter doesn't mutate rows; its output schema is identical to its child's schema
        return child_executor_->GetOutputSchema();
    }
};

#endif