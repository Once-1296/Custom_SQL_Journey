#include<iostream>
#include "custom_catalog.hpp"
#include<vector>

int main()
{
    // Test 2 : Create and Fetch table schema
    std::string filename = "test_2.db";
    std::remove(filename.c_str());
    std::vector<Column> cols = {
        {const_cast<char *>("Col1"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("Col2"), TypeId::VARCHAR, 20, 0, 0}};
    Schema schema(cols);
    {
        catalog cata(filename);
        assert(cata.createTable("table1", schema) == true);
        std::cout<<"Table created !"<<std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::vector<Value>> vals = {{Value(1), Value("Awwab")}, {Value(2), Value("Aribah")}};
        int32_t change;
        assert(cata.InsertRow("table1", vals, &change) == true);
        std::cout<<"Inserted "<<change<<" rows"<<std::endl;
    }
    {
        catalog cata(filename);
        //duplicate
        std::vector<std::vector<Value>> vals = {{Value(1), Value("fksjbwfjk")}};
        int32_t change;
        assert(cata.InsertRow("table1", vals, &change) == true);
        std::cout<<"Inserted "<<change<<" rows"<<std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::vector<Value>> vals = {{Value(3), Value("Aadil")}};
        int32_t change;
        assert(cata.InsertRow("table1", vals, &change) == true);
        std::cout<<"Inserted "<<change<<" rows"<<std::endl;
    }
    {
        catalog cata(filename);
        uint32_t row_count = cata.fetchRowCount("table1");
        std::cout<<"Total Number of rows: "<<row_count<<std::endl;
    }
    std::cout<<"All tests passed !"<<std::endl;
    std::remove(filename.c_str());
}