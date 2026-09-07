// test_day11.cpp
#include "../src/storage/slotted_page.hpp"
#include <cassert>
#include <iostream>

void TestMutations() {
    Page page;
    page.Init();

    uint32_t s0, s1;
    std::string stringA = "Apple";
    std::string stringB = "Banana";

    assert(page.InsertTuple(reinterpret_cast<const uint8_t*>(stringA.data()), stringA.size(), &s0));
    assert(page.InsertTuple(reinterpret_cast<const uint8_t*>(stringB.data()), stringB.size(), &s1));


    // Test In-Place Update (Shrinking data)
    std::string short_apple = "App";
    assert(page.UpdateTuple(s0, reinterpret_cast<const uint8_t*>(short_apple.data()), short_apple.size()) == true);

    uint32_t verify_size;
    const uint8_t* ptr = page.GetTuplePtr(s0, &verify_size);
    assert(std::string(reinterpret_cast<const char*>(ptr), verify_size) == "App");

    // Test Logical Deletion
    assert(page.ApplyDelete(s1) == true);
    page.GetTuplePtr(s1, &verify_size);
    assert(verify_size == 0); // Size must be 0 tracking deletion state

    std::cout << "Day 11 Dynamic Mutations Verification Passed!" << std::endl;
}

int main() {
    TestMutations();
    return 0;
}