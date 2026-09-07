#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>

// 1. Define our fixed-size database record
struct UserRecord
{
    uint32_t id;
    char username[32];
    uint32_t age;
};

// 2. Serialize the struct into a raw byte buffer
std::vector<uint8_t> Serialize(const UserRecord &record)
{
    // Dynamically calculate total size based on the struct components
    constexpr size_t record_size = sizeof(record.id) + sizeof(record.username) + sizeof(record.age);
    std::vector<uint8_t> buffer(record_size);

    size_t offset = 0;

    // 1. Copy ID
    std::memcpy(buffer.data() + offset, &record.id, sizeof(record.id));
    offset += sizeof(record.id);

    // 2. Copy Username
    std::memcpy(buffer.data() + offset, &record.username, sizeof(record.username));
    offset += sizeof(record.username);

    // 3. Copy Age
    std::memcpy(buffer.data() + offset, &record.age, sizeof(record.age));

    return buffer;
}

// 3. Deserialize a raw byte buffer back into a struct
UserRecord Deserialize(const std::vector<uint8_t> &buffer)
{
    UserRecord record;

    // YOUR CODE HERE:
    // Extract the fields back out from the buffer into the 'record' struct
    size_t offset = 0;

    // 1. Copy ID
    std::memcpy(&record.id, buffer.data() + offset, sizeof(record.id));
    offset += sizeof(record.id);

    // 2. Copy Username
    std::memcpy(&record.username, buffer.data() + offset, sizeof(record.username));
    offset += sizeof(record.username);

    // 3. Copy Age
    std::memcpy(&record.age, buffer.data() + offset, sizeof(record.age));

    return record;
}

int main()
{
    // Test Data
    UserRecord original;
    original.id = 1337;
    std::strncpy(original.username, "AliceInByteLand", sizeof(original.username));
    original.age = 21;

    // Run serialization
    std::vector<uint8_t> buffer = Serialize(original);

    // Validate buffer size
    assert(buffer.size() == 40);

    // Run deserialization
    UserRecord recovered = Deserialize(buffer);

    // Verify correctness
    assert(recovered.id == original.id);
    assert(std::strcmp(recovered.username, original.username) == 0);
    assert(recovered.age == original.age);

    std::cout << "🚀 Day 1 Challenge Passed! Data is perfectly preserved in binary." << std::endl;
    return 0;
}