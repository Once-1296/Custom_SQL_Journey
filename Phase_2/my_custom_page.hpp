#ifndef MY_CUSTOM_PAGE_HPP
#define MY_CUSTOM_PAGE_HPP

#include <iostream>
#include <array>
#include <cstring>
#include <cstdint>
#include <cassert>

class Page {
private:
    std::array<uint8_t, 4096> data_buffer;
    static constexpr size_t HEADER_SIZE = sizeof(uint32_t); // 4 bytes for record count

public:
    Page() {
        data_buffer.fill(0); 
    }

    const uint8_t* ReadRawData() const { return data_buffer.data(); }
    uint8_t* WriteRawData() { return data_buffer.data(); }

    uint32_t GetRecordCount() const {
        uint32_t count = 0;
        std::memcpy(&count, data_buffer.data(), HEADER_SIZE);
        return count;
    }

    // Generic append: Accepts a raw pointer to any serialized tuple data and its size
    bool AppendRecord(const uint8_t* record_data, uint32_t record_size) {
        uint32_t current_count = GetRecordCount();

        // Check if data bounds exceed the 4KB limit
        if (HEADER_SIZE + ((current_count + 1) * record_size) > 4096) {
            return false; // Page Full
        }

        // Calculate destination offset for this specific slot
        size_t offset = HEADER_SIZE + (current_count * record_size);

        // Copy raw bytes directly into the page buffer
        std::memcpy(data_buffer.data() + offset, record_data, record_size);

        // Update the header count
        current_count++;
        std::memcpy(data_buffer.data(), &current_count, HEADER_SIZE);
        return true;
    }

    // Zero-Copy Accessor: Returns a direct pointer to the starting byte of a slot
    const uint8_t* GetRecordPtr(uint32_t index, uint32_t record_size) const {
        assert(index < GetRecordCount());
        size_t offset = HEADER_SIZE + (index * record_size);
        return data_buffer.data() + offset;
    }
};

#endif