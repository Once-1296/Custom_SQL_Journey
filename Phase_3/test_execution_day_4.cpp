// test_day12.cpp
#include "my_custom_disk_manager.hpp"
#include "my_custom_buffer_pool_manager.hpp"
#include "catalog.hpp"
#include <cassert>
#include <iostream>

void TestCatalogSystem() {
    std::string filename = "catalog_test.bin";
    std::remove(filename.c_str());

    DiskManager disk(filename);
    BufferPoolManager bpm(disk);
    Catalog catalog(&bpm);

    // 1. Define Table A: "users"
    std::vector<Column> user_cols = {
        {"id", TypeId::INT32, 4, 0},
        {"age", TypeId::INT32, 4, 4}
    };
    Schema user_schema(user_cols);

    // 2. Define Table B: "items"
    std::vector<Column> item_cols = {
        {"item_id", TypeId::INT32, 4, 0}
    };
    Schema item_schema(item_cols);

    // Create the tables
    assert(catalog.CreateTable("users", user_schema) == true);
    assert(catalog.CreateTable("items", item_schema) == true);
    assert(catalog.CreateTable("users", user_schema) == false); // Duplicate block check

    // Verify lookup isolation
    TableMetadata* meta_user = catalog.GetTable("users");
    assert(meta_user != nullptr);
    assert(meta_user->name_ == "users");
    
    TableMetadata* meta_item = catalog.GetTable("items");
    assert(meta_item != nullptr);
    assert(meta_item->first_page_id_ != meta_user->first_page_id_); // Must have unique root blocks!

    std::cout << "Day 12 System Catalog Management Works Cleanly!" << std::endl;
    std::remove(filename.c_str());
}

int main() {
    TestCatalogSystem();
    return 0;
}