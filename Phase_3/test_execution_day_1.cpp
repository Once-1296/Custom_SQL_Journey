// test_day9.cpp
#include "slotted_page.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void TestSlottedPage() {
    SlottedPage page;
    page.Init();

    assert(page.GetSlotCount() == 0);
    assert(page.GetFreeSpacePointer() == 4096);

    std::string data1 = "DatabaseEngine";
    std::string data2 = "SlottedLayoutsRule";

    uint32_t slot1, slot2;
    uint32_t size1, size2;

    // Verify insertions populate contrasting ends of the frame
    assert(page.InsertTuple(reinterpret_cast<const uint8_t*>(data1.data()), data1.size(), &slot1) == true);
    assert(slot1 == 0);

    assert(page.InsertTuple(reinterpret_cast<const uint8_t*>(data2.data()), data2.size(), &slot2) == true);
    assert(slot2 == 1);

    // Verify extraction pointer translation matches source strings
    const uint8_t* ptr1 = page.GetTuplePtr(slot1, &size1);
    std::string res1(reinterpret_cast<const char*>(ptr1), size1);
    assert(res1 == data1);

    const uint8_t* ptr2 = page.GetTuplePtr(slot2, &size2);
    std::string res2(reinterpret_cast<const char*>(ptr2), size2);
    assert(res2 == data2);

    std::cout << "Day 9 Slotted Page Layout Verification Passed!" << std::endl;
}

int main() {
    TestSlottedPage();
    return 0;
}