#ifndef DELETE_EXECUTOR_HPP
#define DELETE_EXECUTOR_HPP

#include "../abstract_executor.hpp"
#include "../abstract_expression.hpp"
#include <map>

class DeleteExecutor : public AbstractExecutor
{
private:
    std::unique_ptr<AbstractExecutor> child_executor_;
public:
    DeleteExecutor(ExecutorContext *exec_ctx, std::unique_ptr<AbstractExecutor> child_executor) : AbstractExecutor(exec_ctx), child_executor_(std::move(child_executor)){}

    void Init() override
    {
        child_executor_->Init();
    }
    bool Next(Tuple *tuple, RID *rid) override
    {
        while (child_executor_->Next(tuple, rid))
        {
            BufferPoolManager *bpm = exec_ctx_->GetBufferPoolManager();
            Page* page = bpm->FetchPage(rid->page_id);
            page->ApplyDelete(rid->slot_num);
            bpm->MarkDirty(rid->page_id);
            return true;
        }
        return false;
    }
    const Schema &GetOutputSchema() const override
    {
        // A delete doesn't mutate rows; its output schema is identical to its child's schema
        return child_executor_->GetOutputSchema();
    }
};

#endif
