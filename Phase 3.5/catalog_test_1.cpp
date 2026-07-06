#include<iostream>
#include "custom_catalog.hpp"
#include<vector>
int main()
{
    std::vector<Column> cols = {
        {"Col1", TypeId::INT32, 4, 0, 1},
        {"Col2",TypeId::VARCHAR,20,0,0}
    };
    Schema schema(cols);
    catalog cata("test_1.db");
    cata.createTable("table1",schema);
}