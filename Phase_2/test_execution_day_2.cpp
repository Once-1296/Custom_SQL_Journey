// test_tuple.cpp
#include "schema.hpp"
#include "tuple.hpp"
#include <cassert>
#include <iostream>
#include <cstdint>
#include <array>
void TestTupleExtraction() {
    // 1. Create the Schema (Same as Day 1)
    std::vector<Column> cols = {
        {"id",       TypeId::INT32,   4, 0},
        {"username", TypeId::VARCHAR, 32, 0},
        {"age",      TypeId::INT32,   4, 0}
    };
    Schema schema(cols);

    // 2. Simulate raw bytes from the BufferPoolManager
    // 4 bytes for ID (105), 32 bytes for Username ("db_admin"), 4 bytes for Age (42)
    std::array<uint8_t, 40> mock_page_slot = {0};
    
    int32_t mock_id = 105;
    const char* mock_username = "db_admin";
    int32_t mock_age = 42;

    std::memcpy(mock_page_slot.data() + 0, &mock_id, 4);
    std::memcpy(mock_page_slot.data() + 4, mock_username, std::strlen(mock_username));
    std::memcpy(mock_page_slot.data() + 36, &mock_age, 4);

    // 3. Create the Tuple Wrapper (Zero-Copy)
    RID rid(1, 0); // Page 1, Slot 0
    Tuple tuple(mock_page_slot.data(), schema.GetTupleSize(), rid);

    // 4. Assertions
    assert(tuple.GetInt32(schema, 0) == 105);
    assert(tuple.GetVarchar(schema, 1) == "db_admin");
    assert(tuple.GetInt32(schema, 2) == 42);
    assert(tuple.GetRID().page_id == 1);

    std::cout << "Day 2 Tuple Tests Passed successfully!" << std::endl;
}

int main() {
    TestTupleExtraction();
    return 0;
}