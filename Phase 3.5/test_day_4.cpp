#include<iostream>
#include "custom_catalog.hpp"
#include<vector>

int main()
{
    // Test 4 : Updating rows (tom maybe :( )
    std::string filename = "test_4.db";
    std::remove(filename.c_str());
    std::vector<Column> cols = {
        {const_cast<char *>("A"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("B"), TypeId::VARCHAR, 20, 0, 0},
        {const_cast<char *>("C"), TypeId::INT32, 4, 0, 0}};
    Schema schema(cols);
    {
        catalog cata(filename);
        assert(cata.createTable("table1", schema) == true);
        std::cout<<"Table created !"<<std::endl;
        std::vector<std::vector<Value>> vals = {{Value(1), Value("Awwab"), Value(161)}, {Value(2), Value("Aribah"), Value(156)}, {Value(3), Value("Aadil"), Value(165)}};
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
        //duplicate
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