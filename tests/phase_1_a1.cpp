#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>

// 1. Define our fixed-size database record
struct UserRecord {
    uint32_t id;
    char username[32]; 
    uint32_t age;
};

// 2. Serialize the struct into a raw byte buffer
std::vector<uint8_t> Serialize(const UserRecord& record) {
    std::vector<uint8_t> buffer(40); // Total size is 40 bytes

    // YOUR CODE HERE: 
    // Copy record.id into bytes 0-3
    // Copy record.username into bytes 4-35
    // Copy record.age into bytes 36-39

    uint32_t mask = UINT8_MAX;
    uint32_t id = record.id;
    for(short i =0; i< 4;i++){
        uint32_t res = mask & id;
        buffer[i] = (uint8_t)res;
        id >>= 8;
    }
    for(short i = 0; i<32;i++){
        buffer[4+i] =(uint8_t)record.username[i];
    }
    uint32_t age = record.age;
    for(short i =0; i< 4;i++){
        uint32_t res = mask & age;
        buffer[36+i] = (uint8_t)res;
        age >>= 8;
    }
    return buffer;
}

// 3. Deserialize a raw byte buffer back into a struct
UserRecord Deserialize(const std::vector<uint8_t>& buffer) {
    UserRecord record;
    
    // YOUR CODE HERE:
    // Extract the fields back out from the buffer into the 'record' struct
    record.id = 0;
    for(short i = 0; i<4;i++){
        uint32_t byte = buffer[i];
        record.id |= byte<<(i*8);
    }
    for(short i = 0;i<32;i++){
        record.username[i] = (char)buffer[4+i];
    }
    record.age = 0;
    for(short i = 0; i<4;i++){
        uint32_t byte = buffer[i+36];
        record.age |= byte<<(i*8);
    }
    return record;
}

int main() {
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