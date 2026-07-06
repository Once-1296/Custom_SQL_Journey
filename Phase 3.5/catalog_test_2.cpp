#include<iostream>
#include "custom_catalog.hpp"
#include<vector>
int main()
{
    std::vector<Column> cols = {
        {const_cast<char*>("Col1"), TypeId::INT32, 4, 0, 1},
        {const_cast<char*>("Col2"),TypeId::VARCHAR,20,0,0}
    };
    Schema schema(cols);
    {
    catalog cata("test_2.db");
    assert(cata.createTable("table1",schema) ==true);
    std::cout<<"Passed Test 1"<<std::endl;
    assert(cata.createTable("table1",schema) == false);
    std::cout<<"Passed Test 2"<<std::endl;
    }
    {
        catalog cata("test_2.db");
        Schema *schem = cata.GetTableSchema("table1");
        assert(*schem == schema);
        std::cout<<"Passed Test 3"<<std::endl;
    }
}