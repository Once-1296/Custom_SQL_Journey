#include <iostream>
#include <array>
#include <cstring>
#include <cstdint>
#include <cassert>
#include "../src/my_custom_page.hpp"

int main()
{
    Page page;
    assert(page.GetRecordCount() == 0);

    // Create a dummy record
    UserRecord user1{1, "Bob", 30};
    UserRecord user2{2, "Alice", 25};

    // Append records
    assert(page.AppendRecord(user1) == true);
    assert(page.AppendRecord(user2) == true);
    assert(page.GetRecordCount() == 2);

    // Retrieve and verify records
    UserRecord fetched1 = page.GetRecord(0);
    UserRecord fetched2 = page.GetRecord(1);

    assert(fetched1.id == 1 && std::strcmp(fetched1.username, "Bob") == 0);
    assert(fetched2.id == 2 && std::strcmp(fetched2.username, "Alice") == 0);

    // Test how many records can fit mathematically: (4096 - 4) / 40 = 102 records
    for (int i = 0; i < 100; ++i)
    {
        UserRecord generic{static_cast<uint32_t>(i + 3), "User", 20};
        page.AppendRecord(generic);
    }

    // The 103rd record should fail because the page is full
    UserRecord overflow{999, "Overflow", 99};
    assert(page.AppendRecord(overflow) == false);

    std::cout << "🔥 Day 2 Challenge Passed! You successfully implemented a database Page Block layout." << std::endl;
    return 0;
}