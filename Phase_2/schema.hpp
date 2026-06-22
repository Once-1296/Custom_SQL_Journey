#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>

// Use an enum for performance-critical type checking
enum class TypeId {
    INT32,
    VARCHAR
};

struct Column {
    std::string name;
    TypeId type;
    uint32_t length; // Size in bytes (e.g., 4 for INT32, N for VARCHAR)
    uint32_t offset; // Pre-calculated byte offset within a tuple
};

class Schema {
private:
    std::vector<Column> columns;
    uint32_t tuple_size = 0; // Total size of a single row in bytes

public:
    // Explicitly define a schema by passing a list of columns
    Schema(const std::vector<Column>& input_columns) : columns(input_columns) {
        uint32_t current_offset = 0;
        for (auto& col : columns) {
            col.offset = current_offset;
            current_offset += col.length;
        }
        tuple_size = current_offset; // Total size is the sum of all column lengths
    }

    ~Schema() = default;

    const std::vector<Column>& GetColumns() const { return columns; }
    
    uint32_t GetTupleSize() const { return tuple_size; }

    uint32_t GetColumnCount() const { return columns.size(); }

    const Column& GetColumn(uint32_t col_idx) const {
        assert(col_idx < columns.size());
        return columns[col_idx];
    }
};

#endif