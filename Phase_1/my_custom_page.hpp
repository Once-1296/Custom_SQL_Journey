#ifndef MY_CUSTOM_PAGE_HPP
#define MY_CUSTOM_PAGE_HPP
#include <iostream>
#include <array>
#include <cstring>
#include <cstdint>
#include <cassert>

// Padding vulnerability
// struct UserRecord
// {
//     uint32_t id;
//     char username[32];
//     uint32_t age;
// };

struct [[gnu::packed]] UserRecord
{
    uint32_t id;
    char username[32];
    uint32_t age;
}; // Prevents compiler padding


class Page
{
private:
    // Exactly 4096 bytes matching typical OS page size
    std::array<uint8_t, 4096> data_buffer;

    // Helper to calculate how large a single record is
    const size_t RECORD_SIZE = sizeof(uint32_t) + 32 + sizeof(uint32_t); // 40 bytes
    const size_t HEADER_SIZE = sizeof(uint32_t);                         // 4 bytes for storing record count

public:
    Page()
    {
        data_buffer.fill(0); // Zero out the page initially
    }
    const uint8_t* ReadRawData() const { return data_buffer.data(); }
    uint8_t* WriteRawData() { return data_buffer.data(); }
    // Returns how many records are currently stored (read from the header)
    uint32_t GetRecordCount() const
    {
        uint32_t count = 0;
        // YOUR CODE HERE: Read the first 4 bytes of data_buffer into 'count' using memcpy
        std::memcpy(&count, data_buffer.data(), HEADER_SIZE);
        return count;
    }

    // Appends a record to the page if there is space
    bool AppendRecord(const UserRecord &record)
    {
        uint32_t current_count = GetRecordCount();

        // Calculate if adding another record exceeds 4096 bytes
        if (HEADER_SIZE + ((current_count + 1) * RECORD_SIZE) > 4096)
        {
            return false; // Page Full!
        }

        // YOUR CODE HERE:
        // 1. Calculate the memory offset where this record should be written
        // 2. Use std::memcpy to serialize the fields (id, username, age) into data_buffer at that offset
        // 3. Increment current_count and update the 4-byte header at offset 0
        size_t offset = HEADER_SIZE + current_count * RECORD_SIZE;

        // 1. Copy ID
        std::memcpy(data_buffer.data() + offset, &record.id, sizeof(record.id));
        offset += sizeof(record.id);

        // 2. Copy Username
        std::memcpy(data_buffer.data() + offset, &record.username, sizeof(record.username));
        offset += sizeof(record.username);

        // 3. Copy Age
        std::memcpy(data_buffer.data() + offset, &record.age, sizeof(record.age));

        current_count++;
        std::memcpy(data_buffer.data(), &current_count, HEADER_SIZE);
        return true;
    }

    // Retrieves a record by its slot index inside this page
    UserRecord GetRecord(uint32_t index) const
    {
        assert(index < GetRecordCount());
        UserRecord record;

        // YOUR CODE HERE:
        // 1. Calculate the memory offset where this specific record index starts
        // 2. Use std::memcpy to read the bytes out into the 'record' fields
        size_t offset = HEADER_SIZE + index * RECORD_SIZE;
        std::memcpy(&record, data_buffer.data() + offset, RECORD_SIZE);
        return record;
    }
};
#endif