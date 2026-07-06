#ifndef CUSTOM_CATALOG
#define CUSTOM_CATALOG

#include <cstdint>
#include <string>
#include <cstring>
#include <cassert>
#include "my_custom_buffer_pool_manager.hpp"
#include "schema.hpp"
#include "filter_executor.hpp"
#include "seq_scan_executor.hpp"
#include "abstract_expression.hpp"
#include "insertion_executor.hpp"

/*
    catalog right now should have 2 main tasks
    create table
        we need to store a data of one table in single slot


    delete table


*/

struct TableData
{
    char name_[64];
    uint32_t first_page_id_;
    uint32_t rows_;
    uint32_t schema_page_id_;
    uint32_t column_count_;
    Schema schema_;
    TableData(char *name, uint32_t first_page_id, uint32_t rows, Schema schema, uint32_t schema_page_id, uint32_t column_count) : first_page_id_(first_page_id),
                                                                                                                                  rows_(rows), schema_(schema), schema_page_id_(schema_page_id), column_count_(column_count)
    {
        std::strncpy(name_, name, sizeof(name));
    };
};

const Schema tab_schema({
    {const_cast<char *>("name"), TypeId::VARCHAR, 64, 0, 1},
    {const_cast<char *>("first_page_id"), TypeId::INT32, 4, 0, 0},
    {const_cast<char *>("rows"), TypeId::INT32, 4, 0, 0},
    {const_cast<char *>("schema_page_id"), TypeId::INT32, 4, 0, 0},
    {const_cast<char *>("column_count"), TypeId::INT32, 4, 0, 0},
});

const Schema col_schema({
    {const_cast<char *>("name"), TypeId::VARCHAR, 32, 0, 1},
    {const_cast<char *>("length"), TypeId::INT32, 4, 0, 0},
    {const_cast<char *>("type"), TypeId::INT32, 4, 0, 0},
    {const_cast<char *>("is_in_candidate_key"), TypeId::INT32, 4, 0, 0},
});

class catalog
{
private:
    std::string filename_;
    BufferPoolManager *bpm_;
    DiskManager *disk_manager_;
    uint32_t first_page;

    // converts table data into raw bytes to store in page
    uint8_t *getTableBytes(TableData &tableData, uint32_t *tuple_size)
    {
        uint32_t header = sizeof(tableData.name_) + sizeof(tableData.first_page_id_) + sizeof(tableData.rows_) + sizeof(tableData.schema_page_id_) + sizeof(tableData.column_count_);
        uint8_t *result = new uint8_t[header];

        uint32_t offset = 0;
        std::memcpy(result + offset, tableData.name_, sizeof(tableData.name_));
        offset += sizeof(tableData.name_);
        std::memcpy(result + offset, &tableData.first_page_id_, sizeof(tableData.first_page_id_));
        offset += sizeof(tableData.first_page_id_);
        std::memcpy(result + offset, &tableData.rows_, sizeof(tableData.rows_));
        offset += sizeof(tableData.rows_);
        std::memcpy(result + offset, &tableData.schema_page_id_, sizeof(tableData.schema_page_id_));
        offset += sizeof(tableData.schema_page_id_);
        std::memcpy(result + offset, &tableData.column_count_, sizeof(tableData.column_count_));
        offset += sizeof(tableData.column_count_);
        assert(offset == header);
        *tuple_size = offset;
        return result;
    }

public:
    catalog(std::string filename)
    {
        disk_manager_ = new DiskManager(filename);
        bpm_ = new BufferPoolManager(*disk_manager_);
        if (disk_manager_->GetTotalPages() > 0)
            first_page = 0;
        else
            first_page = bpm_->NewPage();
    }
    ~catalog()
    {
        // Add this to properly clean up and trigger the BPM to flush to disk
        delete bpm_;
        delete disk_manager_;
    }
    bool createTable(std::string tableName, Schema &schema)
    {
        assert(tableName.length() < 64);

        // check if table exists
        if (GetTableSchema(tableName) != nullptr)
            return false;

        char name[64];
        std::strcpy(name, tableName.c_str());
        uint32_t schema_page_id = bpm_->NewPage();
        uint32_t column_count = schema.GetColumnCount();
        uint32_t first_page_id = bpm_->NewPage();
        uint32_t rows = 0;
        // make table metadata
        TableData tableData(name, first_page_id, rows, schema, schema_page_id, column_count);
        uint32_t tuple_size = 0;

        // get raw bytes for metadata
        uint8_t *data = getTableBytes(tableData, &tuple_size);
        Tuple tuple(data, tuple_size, RID(0, 0));
        RID rid;
        // insert
        // Use insert executor later
        ExecutorContext *ctx = new ExecutorContext(*bpm_);
        InsertionExecutor insertor(ctx, tab_schema, first_page);
        insertor.Init();
        insertor.Next(&tuple, &rid);
        InsertionExecutor col_insertor(ctx, col_schema, schema_page_id);
        for (auto &col : schema.GetColumns())
        {
            uint32_t col_size;
            uint8_t *col_data = schema.getColumnBytes(col, &col_size);
            Tuple tuple(col_data, col_size, RID(0, 0));
            RID rid;
            col_insertor.Init();
            col_insertor.Next(&tuple, &rid);
        }
        return true;
    }

    Schema *GetTableSchema(std::string Name, uint32_t *out_schema_page_id = nullptr, uint32_t *out_first_page_id = nullptr)
    {
        auto col_expr = std::make_unique<ColumnValueExpression>(0);
        auto const_expr = std::make_unique<ConstantValueExpression>(Value(Name));
        ExecutorContext *ctx = new ExecutorContext(*bpm_);
        FilterExecutor FE(ctx, std::make_unique<SeqScanExecutor>(SeqScanExecutor(ctx, tab_schema, 0, first_page)), std::make_unique<EqualExpression>(EqualExpression(std::move(col_expr), std::move(const_expr))));
        FE.Init();
        Tuple tuple;
        RID rid;
        bool tableExists = FE.Next(&tuple, &rid);
        if (!tableExists)
        {
            if (out_schema_page_id != nullptr)
            {
                *out_schema_page_id = 0xFFFFFFFF;
            }
            if (out_first_page_id != nullptr)
            {
                *out_first_page_id = 0xFFFFFFFF;
            }
            return nullptr;
        }
        uint32_t schema_page_id = tuple.GetInt32(tab_schema, 3); // schema_page_id is at 3
        uint32_t first_page_id = tuple.GetInt32(tab_schema, 1);
        SeqScanExecutor schema_fetch(ctx, col_schema, 0, schema_page_id);
        schema_fetch.Init();
        std::vector<Column> cols;
        while (schema_fetch.Next(&tuple, &rid))
        {
            std::string col_name = tuple.GetVarchar(col_schema, 0);
            uint32_t col_length = tuple.GetInt32(col_schema, 1);
            uint32_t col_type = tuple.GetInt32(col_schema, 2);
            bool is_in_candidate_key = tuple.GetInt32(col_schema, 3) == 1;
            TypeId type = (col_type == 1) ? TypeId::INT32 : TypeId::VARCHAR;
            char c_name[32];
            std::strcpy(c_name, col_name.c_str());
            cols.push_back(Column(c_name, type, col_length, 0, is_in_candidate_key));
        }
        if (out_schema_page_id != nullptr)
        {
            *out_schema_page_id = schema_page_id;
        }
        if (out_first_page_id != nullptr)
        {
            *out_first_page_id = first_page_id;
        }
        Schema *schema = new Schema(cols);
        return schema;
    }

    bool InsertRow(std::string tableName, Tuple *data_ptr)
    {
        uint32_t schema_page_id, first_page_id;
        Schema* schema = GetTableSchema(tableName, &schema_page_id, &first_page_id);
        if(schema == nullptr)
        {
            return false;
        }
        ExecutorContext *ctx = new ExecutorContext(*bpm_);
        InsertionExecutor insertor(ctx, *schema, first_page_id);
        insertor.Init();
        RID rid;
        insertor.Next(data_ptr, &rid);
        return true;
    }

    std::vector<Tuple> Query()
    {
        
    }
};

#endif