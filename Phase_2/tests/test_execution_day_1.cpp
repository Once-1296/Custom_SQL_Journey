// test_schema.cpp
#include "../src/types/schema.hpp"
#include <cassert>

void TestUserRecordSchema() {
    // Recreating UserRecord: { uint32_t id; char username[32]; uint32_t age; }
    std::vector<Column> cols = {
        {"id",       TypeId::INT32,   4, 0},
        {"username", TypeId::VARCHAR, 32, 0},
        {"age",      TypeId::INT32,   4, 0}
    };

    Schema schema(cols);

    // Assertions for correctness
    assert(schema.GetColumnCount() == 3);
    assert(schema.GetTupleSize() == 40); // 4 + 32 + 4 = 40 bytes (Matches your Phase 1 RECORD_SIZE)

    // Check pre-calculated offsets
    assert(schema.GetColumn(0).offset == 0);  // id starts at byte 0
    assert(schema.GetColumn(1).offset == 4);  // username starts at byte 4
    assert(schema.GetColumn(2).offset == 36); // age starts at byte 36

    std::cout << "Day 1 Schema Tests Passed successfully! Total Tuple Size: " 
              << schema.GetTupleSize() << " bytes." << std::endl;
}

int main() {
    TestUserRecordSchema();
    return 0;
}