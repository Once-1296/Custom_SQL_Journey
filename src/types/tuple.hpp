#ifndef TUPLE_HPP
#define TUPLE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <algorithm>
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
    const uint8_t *data_ptr_; // Pointer to data (owned or zero-copy)
    uint32_t size_;           // Size of the tuple in bytes
    RID rid_;                 // Physical location of this tuple
    bool allocated_{false};   // TRUE if this Tuple owns the memory and must delete it

public:
    Tuple() : data_ptr_(nullptr), size_(0), allocated_(false) {}

    // 1. STANDARD CONSTRUCTOR (Zero-Copy)
    // Used by Buffer Pool to point to an existing page without copying
    Tuple(const uint8_t *data_ptr, uint32_t size, RID rid)
        : data_ptr_(data_ptr), size_(size), rid_(rid), allocated_(false) {}

    // 2. COPY CONSTRUCTOR (Deep Copy)
    // Triggered when doing: output.push_back(tuple);
    Tuple(const Tuple &other) : size_(other.size_), rid_(other.rid_), allocated_(false) {
        if (other.data_ptr_ != nullptr && other.size_ > 0) {
            // Allocate new memory on the heap
            uint8_t *copy = new uint8_t[size_];
            // Copy the actual bytes over
            std::memcpy(copy, other.data_ptr_, size_);
            data_ptr_ = copy;
            allocated_ = true; // Mark that WE own this memory
        } else {
            data_ptr_ = nullptr;
        }
    }

    // 3. COPY ASSIGNMENT OPERATOR (Deep Copy)
    // Triggered when doing: tuple1 = tuple2;
    Tuple& operator=(const Tuple &other) {
        if (this == &other) return *this; // Protect against self-assignment

        // Free our current memory if we own it
        if (allocated_ && data_ptr_ != nullptr) {
            delete[] const_cast<uint8_t*>(data_ptr_);
        }

        size_ = other.size_;
        rid_ = other.rid_;
        
        if (other.data_ptr_ != nullptr && other.size_ > 0) {
            uint8_t *copy = new uint8_t[size_];
            std::memcpy(copy, other.data_ptr_, size_);
            data_ptr_ = copy;
            allocated_ = true;
        } else {
            data_ptr_ = nullptr;
            allocated_ = false;
        }

        return *this;
    }

    // 4. DESTRUCTOR
    ~Tuple() {
        // Only delete the memory if we allocated it!
        // (Do not delete if it belongs to the Buffer Pool)
        if (allocated_ && data_ptr_ != nullptr) {
            delete[] const_cast<uint8_t*>(data_ptr_);
        }
    }

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
        size_t actual_len = strnlen(start, col.length);
        return std::string(start, actual_len);
    }
};


bool tupleSort(std::vector<Tuple> &tuples, std::vector<std::pair<uint32_t, uint32_t>>&order, Schema *&schema, std::string &Message)
{
    try{
        std::sort(tuples.begin(), tuples.end(),[&schema, &order](const Tuple &A, const Tuple &B)->bool{
            for(uint32_t i = 0; i < order.size();i++)
            {
                uint32_t cInd = order[i].first, option = order[i].second;
                TypeId type = schema->GetColumn(cInd).type;
                if(type == TypeId::INT32)
                {
                    int32_t av = A.GetInt32(*schema, cInd);
                    int32_t bv = B.GetInt32(*schema, cInd);
                    if(av != bv || i + 1 == order.size())
                    {
                        return (option == 0) ? av < bv : av > bv;
                    }
                }
                else
                {
                    std::string av = A.GetVarchar(*schema, cInd);
                    std::string bv = B.GetVarchar(*schema, cInd);
                    if(av != bv || i + 1 == order.size())
                    {
                        return (option == 0) ? av < bv : av > bv;
                    }
                }
            }
            return false;
        });
    }
    catch(...)
    {
        Message = "Unexpected error in sorting rows.";
        return false;
    }
    return true;
}

#endif