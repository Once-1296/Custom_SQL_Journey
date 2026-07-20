#include <iostream>
#include "custom_catalog.hpp"
#include <vector>
int main()
{
    // Test 1 : Create and Fetch table schema
    std::string filename = "test_1.db";
    std::remove(filename.c_str());
    std::vector<Column> cols = {
        {const_cast<char *>("Col1"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("Col2"), TypeId::VARCHAR, 20, 0, 0}};
    Schema schema(cols);
    std::vector<Column> cols2 = {
        {const_cast<char *>("name"), TypeId::VARCHAR, 16, 0, 1},
        {const_cast<char *>("id"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("age"), TypeId::INT32, 4, 0, 0}
    };
    Schema schema2(cols2);
    {
        catalog cata(filename);
        assert(cata.createTable("table1", schema) == true);
        std::cout<<"Table created !"<<std::endl;
    }
    {
        catalog cata(filename);
        assert(cata.createTable("table1", schema) == false);
        Schema *fetch_schema = cata.GetTableSchema("table1");
        assert(fetch_schema != nullptr);
        assert(*fetch_schema == schema);
        std::cout<<"Schema Matched !"<<std::endl;
    }
    {
        catalog cata(filename);
        assert(cata.createTable("table2",schema2) == true);
        std::cout<<"Second Table Created !"<<std::endl;
        Schema *fetch_schema = cata.GetTableSchema("table2");
        assert(fetch_schema != nullptr);
        assert(*fetch_schema == schema2);
        std::cout<<"Second Schema Matched !"<<std::endl;
    }
    {
        catalog cata(filename);
        assert(cata.GetTableSchema("non-existent-table") == nullptr);
        std::cout<<"Non existent case handled !"<<std::endl;
    }
    std::cout<<"All tests passed !"<<std::endl;
    std::remove(filename.c_str());
}