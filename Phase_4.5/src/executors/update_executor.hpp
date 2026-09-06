#ifndef UPDATE_EXECUTOR_HPP
#define UPDATE_EXECUTOR_HPP

#include "../abstract_executor.hpp"
#include "../abstract_expression.hpp"
#include <map>

class UpdateExecutor : public AbstractExecutor
{
private:
    std::unique_ptr<AbstractExecutor> child_executor_;
    std::map<std::string, Value> col_value_map_;

public:
    UpdateExecutor(ExecutorContext *exec_ctx, std::unique_ptr<AbstractExecutor> child_executor, std::map<std::string, Value> col_value_map) : AbstractExecutor(exec_ctx), child_executor_(std::move(child_executor)), col_value_map_(col_value_map) {}

    void Init() override
    {
        child_executor_->Init();
    }
    bool Next(Tuple *tuple, RID *rid) override
    {
        while (child_executor_->Next(tuple, rid))
        {
            Schema schema = GetOutputSchema();
            uint32_t col_count = schema.GetColumnCount(), tuple_size = schema.GetTupleSize();
            uint8_t *new_data = new uint8_t[tuple_size];
            uint32_t offset = 0;
            for (uint32_t i = 0; i < col_count; i++)
            {
                const Column &col = schema.GetColumn(i);
                std::string name = col.name;
                if (col_value_map_.contains(name))
                {
                    Value val = col_value_map_[name];
                    if (col.type == TypeId::INT32)
                    {
                        int32_t num = val.AsInt32();
                        std::memcpy(new_data + offset, &num, col.length);
                    }
                    else if (col.type == TypeId::VARCHAR)
                    {
                        std::string str = val.AsVarchar();
                        char *chr_arr = new char[col.length];
                        std::strncpy(chr_arr, str.c_str(), col.length);
                        std::memcpy(new_data + offset, chr_arr, col.length);
                    }
                }
                else
                {
                    if (col.type == TypeId::INT32)
                    {
                        int32_t num = tuple->GetInt32(schema, i);
                        std::memcpy(new_data + offset, &num, col.length);
                    }
                    else if (col.type == TypeId::VARCHAR)
                    {
                        std::string str = tuple->GetVarchar(schema, i);
                        char *chr_arr = new char[col.length];
                        std::strncpy(chr_arr, str.c_str(), col.length);
                        std::memcpy(new_data + offset, chr_arr, col.length);
                    }
                }
                offset += col.length;
            }
            BufferPoolManager *bpm = exec_ctx_->GetBufferPoolManager();
            Page *page = bpm->FetchPage(rid->page_id);
            page->UpdateTuple(rid->slot_num, new_data, offset);
            bpm->MarkDirty(rid->page_id);
            return true;
        }
        return false;
    }
    const Schema &GetOutputSchema() const override
    {
        // A update doesn't mutate rows; its output schema is identical to its child's schema
        return child_executor_->GetOutputSchema();
    }
};

#endif
