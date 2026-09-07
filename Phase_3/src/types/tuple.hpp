#ifndef TUPLE_HPP
#define TUPLE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <cassert>
#include "schema.hpp"

// Uniquely identifies a physical record on disk
struct RID
{
    uint32_t page_id;
    uint32_t slot_num;

    RID() : page_id(0), slot_num(0) {}
    RID(uint32_t page_id, uint32_t slot_num) : page_id(page_id), slot_num(slot_num) {}
};

class Tuple
{
private:
    const uint8_t *data_ptr_; // Zero-copy pointer to the buffer pool page
    uint32_t size_;           // Size of the tuple in bytes
    RID rid_;                 // Physical location of this tuple

public:
    Tuple() : data_ptr_(nullptr), size_(0) {}

    // Initialize the tuple with a pointer to the raw page data
    Tuple(const uint8_t *data_ptr, uint32_t size, RID rid)
        : data_ptr_(data_ptr), size_(size), rid_(rid) {}

    ~Tuple() = default;

    RID GetRID() const { return rid_; }
    const uint8_t *GetData() const { return data_ptr_; }
    const uint32_t GetSize() const { return size_; }

    // Extracts an integer field using the Schema's pre-calculated offset
    int32_t GetInt32(const Schema &schema, uint32_t col_idx) const
    {
        assert(data_ptr_ != nullptr);
        const Column &col = schema.GetColumn(col_idx);
        assert(col.type == TypeId::INT32);

        int32_t value;
        // YOUR CODE HERE: Use memcpy to copy 'col.length' bytes from
        // (data_ptr_ + col.offset) into 'value'.
        std::memcpy(&value, data_ptr_ + col.offset, col.length);
        return value;
    }

    // Extracts a string field using the Schema's pre-calculated offset
    std::string GetVarchar(const Schema &schema, uint32_t col_idx) const
    {
        assert(data_ptr_ != nullptr);
        const Column &col = schema.GetColumn(col_idx);
        assert(col.type == TypeId::VARCHAR);

        const char *start = reinterpret_cast<const char *>(data_ptr_ + col.offset);

        // Use strnlen to safely find the first null terminator up to col.length
        size_t actual_len = ::strnlen(start, col.length);
        return std::string(start, actual_len);
    }
};

#endif