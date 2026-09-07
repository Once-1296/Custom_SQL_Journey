#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include <iostream>
#include <vector>
#include<array>
#include <string>
#include <cstdint>
#include <cassert>
#include <cstring>


// Use an enum for performance-critical type checking
enum class TypeId {
    INT32,
    VARCHAR
};

struct Column {
    char name[32];
    TypeId type;
    uint32_t length; // Size in bytes (e.g., 4 for INT32, N for VARCHAR)
    uint32_t offset; // Pre-calculated byte offset within a tuple, represents the size of all columns before it
    bool is_in_candidate_key;
    Column (char *name_, TypeId type_, uint32_t length_, uint32_t offset_, bool is_in_candidate_key_) {
        std::strncpy(name, name_, sizeof(name));
        name[sizeof(name) - 1] = '\0'; // Ensure null-termination
        type = type_;
        length = length_;
        offset = offset_;
        is_in_candidate_key = is_in_candidate_key_;
    }
    auto operator<=>(const Column&) const = default;
};

class Schema {
private:
    std::vector<Column> columns;
    const uint32_t col_size = 44;
    uint32_t tuple_size = 0; // Total size of a single row in bytes

public:
    // Explicitly define a schema by passing a list of column
    Schema(const std::vector<Column>& input_columns = {}) : columns(input_columns) {
        uint32_t current_offset = 0;
        for (auto& col : columns) {
            col.offset = current_offset;
            current_offset += col.length;
        }
        tuple_size = current_offset; // Total size is the sum of all column lengths
    }

    ~Schema() = default;

    const bool operator==(const Schema &other) const{
        if(this->tuple_size != other.tuple_size)return false;
        if(this->GetColumnCount() != other.GetColumnCount())return false;
        for(int i = 0;i<this->GetColumnCount();i++)
        {
            if(this->GetColumn(i)!=other.GetColumn(i))return false;
        }
        return true;
    }

    const std::vector<Column>& GetColumns() const { return columns; }
    
    uint32_t GetTupleSize() const { return tuple_size; }

    uint32_t GetColumnCount() const { return columns.size(); }

    const Column& GetColumn(uint32_t col_idx) const {
        assert(col_idx < columns.size());
        return columns[col_idx];
    }


    uint8_t *getColumnBytes(Column col, uint32_t* out_size)
    {
        std::array<uint8_t,44>buffer;
        uint32_t offset = 0;
        std::memcpy(buffer.data()+offset, col.name, sizeof(col.name));
        offset += sizeof(col.name);
        std::memcpy(buffer.data()+offset, &col.length, sizeof(col.length));
        offset += sizeof(col.length);
        uint32_t type = (col.type == TypeId::INT32)?1:0;
        std::memcpy(buffer.data()+offset, &type, sizeof(type));
        offset += sizeof(type);
        uint32_t is_in_candidate_key = (col.is_in_candidate_key)?1:0;
        std::memcpy(buffer.data()+offset, &is_in_candidate_key, sizeof(is_in_candidate_key));
        offset += sizeof(is_in_candidate_key);
        assert(offset == buffer.size());
        uint8_t *result = new uint8_t[buffer.size()];
        std::memcpy(result, buffer.data(), buffer.size());
        *out_size =offset;
        return result;
    }
    uint8_t *getSchemaBytes(uint32_t *out_size)
    {
        uint32_t total_cols = GetColumnCount();
        std::vector<uint8_t>buffer(4 + 38*total_cols);
        uint32_t offset = 0;
        std::memcpy(buffer.data() + offset, &total_cols, sizeof(total_cols));
        offset += sizeof(total_cols);
        for(auto&col:columns)
        {
            uint32_t col_size;
            uint8_t* col_data = getColumnBytes(col,&col_size);
            std::memcpy(buffer.data() + offset,col_data, col_size);
            offset += col_size;
        }
        uint8_t *result = new uint8_t[buffer.size()];
        std::memcpy(result, buffer.data(), buffer.size());
        *out_size = buffer.size();
        return result; 
    }
};

#endif