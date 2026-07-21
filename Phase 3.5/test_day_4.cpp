#include <iostream>
#include "custom_catalog.hpp"
#include <vector>

int main()
{
    // Test 4 : Updating rows 
    std::string filename = "test_4.db";
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
        std::vector<std::string> tcols = {"B", "C"};
        auto expr = std::make_unique<ComparisonExpression>(ComparisonExpression(std::move(std::make_unique<ColumnValueExpression>(ColumnValueExpression(2))), std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(160))))));
        auto res = cata.Query("table1", tcols, std::move(expr), {"Name", "Height"});
        assert(std::get<0>(res) == true);
        std::cout << "Entries with height > 160: \n";
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        catalog cata(filename);
        auto expr = std::make_unique<EqualExpression>(EqualExpression(std::move(std::make_unique<ColumnValueExpression>(ColumnValueExpression(1))), std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value("Aribah"))))));
        uint32_t affected_rows;
        assert(cata.UpdateRow("table1", {{"C", Value(162)}}, std::move(expr), &affected_rows) == true);
        std::cout << "Affected " << affected_rows << " rows" << std::endl;
        std::vector<std::string> tcols = {"B", "C"};
        auto cexpr = std::make_unique<ComparisonExpression>(ComparisonExpression(std::move(std::make_unique<ColumnValueExpression>(ColumnValueExpression(2))), std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(160))))));
        auto res = cata.Query("table1", tcols, std::move(cexpr), {"Name", "Height"});
        assert(std::get<0>(res) == true);
        std::cout << "Entries with height > 160: \n";
        print_table(std::get<1>(res), std::get<2>(res));
    }
    {
        catalog cata(filename);
        uint32_t affected_rows;
        assert(cata.UpdateRow("table1", {{"D", Value(19)}}, std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)))), &affected_rows) == true);
        std::cout << "Affected " << affected_rows << " rows" << std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::string> tcols = {"B", "C", "D"};
        auto res = cata.Query("table1", tcols, std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(Value(1)))), {"Name", "Height", "Age"});
        assert(std::get<0>(res) == true);
        std::cout << "All Entries\n";
        print_table(std::get<1>(res), std::get<2>(res));
    }
    std::cout << "All tests passed !" << std::endl;
    std::remove(filename.c_str());
}