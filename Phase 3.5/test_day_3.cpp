#include <iostream>
#include "custom_catalog.hpp"
#include <vector>
int main()
{
    // Test 3 : querying
    std::string filename = "test_3.db";
    std::remove(filename.c_str());
    std::vector<Column> cols = {
        {const_cast<char *>("Id"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("name"), TypeId::VARCHAR, 20, 0, 0}};
    Schema schema(cols);
    {
        catalog cata(filename);
        assert(cata.createTable("student", schema) == true);
        std::cout << "Passed Test 1" << std::endl;
        assert(cata.createTable("student", schema) == false);
        std::cout << "Passed Test 2" << std::endl;
    }
    {
        catalog cata(filename);
        Schema *schem = cata.GetTableSchema("student");
        assert(*schem == schema);
        std::cout << "Passed Test 3" << std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::vector<Value>> values = {{Value(60), Value("Awwab")}};
        assert(cata.InsertRow("student", values) == true);
        assert(cata.InsertRow("nonexist", values) == false);
        values = {{Value("19"), Value("Awwab")}};
        assert(cata.InsertRow("student", values) == false);
        values ={{Value(47), Value("Aadil")},{Value(50), Value("Aribah")}};
        assert(cata.InsertRow("student", values) == true);
        std::cout << "Inserted Values" << std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::string> cols = {"name", "Id"};
        auto res = cata.Query("student", cols);
        assert(std::get<0>(res) == true);
        std::cout << "Fetched Values" << std::endl;
        Schema schema = std::get<1>(res);
        uint32_t n = schema.GetColumnCount();
        std::vector<Tuple> vec = std::get<2>(res);
        for (uint32_t j = 0; j < vec.size(); j++)
        {
            Tuple tuple = vec[j];
            for (uint32_t i = 0; i < n; i++)
            {
                const Column &col = schema.GetColumn(i);
                int32_t ival;
                std::string sval;
                if (col.type == TypeId::INT32)
                {
                    ival = tuple.GetInt32(schema, i);
                }
                else
                {
                    // std::cout << tuple.GetVarchar(schema, i) << std::endl;
                    sval = tuple.GetVarchar(schema, i);
                }
                std::cout << "Column: " << (col.name) << " , Value: ";
                if (col.type == TypeId::INT32)
                {
                    std::cout << ival;
                }
                else
                {
                    std::cout << sval;
                }
                std::cout << std::endl;
            }
        }
    }
    {
        catalog cata(filename);
        std::vector<std::string> cols = {"name", "Id"};
        auto col_expr = std::make_unique<ColumnValueExpression>(0);
        auto const_expr = std::make_unique<ConstantValueExpression>(Value(50));
        ComparisonExpression comp_expr(std::move(col_expr), std::move(const_expr));
        auto res = cata.Query("student", cols, std::move(std::make_unique<ComparisonExpression>(std::move(comp_expr))), {"Identity", "Naam"});
        assert(std::get<0>(res) == true);
        std::cout << "Fetched Values Where Id > 50" << std::endl;
        Schema schema = std::get<1>(res);
        uint32_t n = schema.GetColumnCount();
        std::vector<Tuple> vec = std::get<2>(res);
        for (uint32_t j = 0; j < vec.size(); j++)
        {
            Tuple tuple = vec[j];
            for (uint32_t i = 0; i < n; i++)
            {
                const Column &col = schema.GetColumn(i);
                int32_t ival;
                std::string sval;
                if (col.type == TypeId::INT32)
                {
                    ival = tuple.GetInt32(schema, i);
                }
                else
                {
                    // std::cout << tuple.GetVarchar(schema, i) << std::endl;
                    sval = tuple.GetVarchar(schema, i);
                }
                std::cout << "Column: " << (col.name) << " , Value: ";
                if (col.type == TypeId::INT32)
                {
                    std::cout << ival;
                }
                else
                {
                    std::cout << sval;
                }
                std::cout << std::endl;
            }
        }
    }

    std::remove(filename.c_str());
}