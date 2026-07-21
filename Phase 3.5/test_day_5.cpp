#include <iostream>
#include "custom_catalog.hpp"
#include <vector>

int main()
{
    // Test 5 : DELLLLEEEETTTTEE
    std::string filename = "test_5.db";
    std::remove(filename.c_str());
    std::vector<Column> cols = {
        {const_cast<char *>("A"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("B"), TypeId::VARCHAR, 20, 0, 0},
        {const_cast<char *>("C"), TypeId::INT32, 4, 0, 0},
        {const_cast<char *>("D"), TypeId::INT32, 4, 0, 0}};
    Schema schema(cols);
    {
        catalog cata(filename);
        assert(cata.createTable("table1", schema) == true);
        std::cout << "Table created !" << std::endl;
        std::vector<std::vector<Value>> vals = {{Value(1), Value("Awwab"), Value(161), Value(18)}, {Value(2), Value("Aribah"), Value(156), Value(17)}, {Value(3), Value("Aadil"), Value(168), Value(17)}};
        int32_t change;
        assert(cata.InsertRow("table1", vals, &change) == true);
        std::cout << "Inserted " << change << " rows" << std::endl;
        std::vector<std::string> tcols = {"B", "C", "D"};
        auto res = cata.Query("table1", tcols, std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)))), {"Name", "Height", "Age"});
        assert(std::get<0>(res) == true);
        std::cout << "All Entries\n";
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        catalog cata(filename);
        auto expr = std::make_unique<EqualExpression>(EqualExpression(std::move(std::make_unique<ColumnValueExpression>(ColumnValueExpression(1))), std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value("Awwab"))))));
        int32_t deleted_rows;
        assert(cata.DeleteRow("table1", std::move(expr), &deleted_rows) == true);
        std::cout << "Deleted " << -deleted_rows << " rows" << std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::string> tcols = {"B", "C", "D"};
        auto res = cata.Query("table1", tcols, std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)))), {"Name", "Height", "Age"});
        assert(std::get<0>(res) == true);
        std::cout << "All Entries\n";
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        catalog cata(filename);
        int32_t deleted_rows;
        assert(cata.DeleteRow("table1", std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)))), &deleted_rows) == true);
        std::cout << "Deleted " << -deleted_rows << " rows" << std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::string> tcols = {"B", "C", "D"};
        auto res = cata.Query("table1", tcols, std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)))), {"Name", "Height", "Age"});
        assert(std::get<0>(res) == true);
        assert(std::get<2>(res).size() == 0);
        std::cout << "No Entries left\n";
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        catalog cata(filename);
        auto res = cata.GetDBMeta();
        assert(res.empty() == false);
        std::cout << "DB meta" << std::endl;
        print_table(tab_schema, res);
        assert(cata.DeleteTable("table1") == true);
        std::cout << "Table deleted successfully !" << std::endl;
    }
    {
        catalog cata(filename);
        auto res = cata.GetDBMeta();
        assert(res.empty() == true);
        std::cout << "DB meta after delete" << std::endl;
        print_table(tab_schema, res);
    }
    std::vector<Column> tcols = {
        {const_cast<char *>("Col1"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("Col2"), TypeId::VARCHAR, 20, 0, 0}};
    Schema testschema(tcols);
    {
        catalog cata(filename);
        cata.createTable("dummy", testschema);
        auto res = cata.GetDBMeta();
        assert(res.empty() == false);
        std::cout << "DB meta after dummy create" << std::endl;
        print_table(tab_schema, res);
    }
    {
        catalog cata(filename);
        std::vector<std::vector<Value>> vals = {{Value(1), Value("Awwab")}, {Value(2), Value("Aribah")}};
        int32_t insert_count;
        assert(cata.InsertRow("dummy", vals, &insert_count) ==  true);
        std::cout<<"Inserted "<<insert_count<<" rows"<<std::endl;
    }
    {
        catalog cata(filename);
        auto res = cata.GetSchemaCols("dummy");
        assert(std::get<0>(res) == true);
        std::cout<<"Columns of dummy table"<<std::endl;
        print_table(col_schema, std::get<2>(res));
    }
    {
        catalog cata(filename);
        std::vector<std::string> tcols = {"Col1","Col2"};
        auto res = cata.Query("dummy", tcols, std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)))), {"Id", "Name"});
        assert(std::get<0>(res) == true);
        std::cout << "All Entries\n";
        print_table(std::get<1>(res), std::get<2>(res));
    }
    std::cout << "All tests passed !" << std::endl;
    std::remove(filename.c_str());
}