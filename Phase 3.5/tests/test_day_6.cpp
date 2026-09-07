#include <iostream>
#include <iomanip>
#include <vector>
#include "../src/custom_catalog.hpp"

int main()
{
    std::string filename = "test_6.db";
    std::remove(filename.c_str());

    std::vector<Column> cols = {
        {const_cast<char *>("Id"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("name"), TypeId::VARCHAR, 20, 0, 0}};
    Schema schema(cols);

    {
        catalog cata(filename);
        assert(cata.createTable("student", schema) == true);
        std::cout << "Passed Test 1: Created Table" << std::endl;
        assert(cata.createTable("student", schema) == false);
        std::cout << "Passed Test 2: Duplicate Table Rejection" << std::endl;
        auto res = cata.GetDBMeta();
        assert(res.empty() == false);
        std::cout<<"DB meta"<<std::endl;
        print_table(tab_schema, res);
    }
    {
        catalog cata(filename);
        Schema *schem = cata.GetTableSchema("student");
        assert(*schem == schema);
        std::cout << "Passed Test 3: Schema Match" << std::endl;
        delete schem; // Clean up memory!
    }

    {
        catalog cata(filename);
        std::vector<std::vector<Value>> values = {{Value(60), Value("Awwab")}};
        assert(cata.InsertRow("student", values) == true);

        assert(cata.InsertRow("nonexist", values) == false);

        values = {{Value("19"), Value("Awwab")}}; // Invalid type (String for INT32)
        assert(cata.InsertRow("student", values) == false);

        values = {{Value(47), Value("Aadil")}, {Value(50), Value("Aribah")}};
        int32_t cnt;
        assert(cata.InsertRow("student", values, &cnt) == true);
        std::cout<<"Inserted " << cnt<<" rows"<<std::endl;
        // Duplicate insert
        assert(cata.InsertRow("student", values, &cnt) == true);
        assert(cnt == 0);
        std::cout<<"Inserted " << cnt<<" rows"<<std::endl;
        std::cout << "Passed Test 4: Inserted Initial Values" << std::endl;
    }

    {
        catalog cata(filename);
        std::vector<std::string> target_cols = {"Id", "name"};
        auto res = cata.Query("student", target_cols);
        assert(std::get<0>(res) == true);

        std::cout << "\nFetched All Values:" << std::endl;
        print_table(std::get<1>(res), std::get<2>(res));
    }

    {
        catalog cata(filename);
        std::vector<std::string> target_cols = {"Id", "name"};

        // Id > 48
        auto col_expr = std::make_unique<ColumnValueExpression>(0); // 0 corresponds to "Id" in the base schema
        auto const_expr = std::make_unique<ConstantValueExpression>(Value(48));
        ComparisonExpression comp_expr(std::move(col_expr), std::move(const_expr));

        auto res = cata.Query("student", target_cols, std::make_unique<ComparisonExpression>(std::move(comp_expr)));
        assert(std::get<0>(res) == true);

        std::cout << "\nFetched Values Where Id > 48:" << std::endl;
        print_table(std::get<1>(res), std::get<2>(res));
    }

    // --- NEW TEST: UPDATE ROW ---
    {
        catalog cata(filename);

        // Condition: Id == 47 (Aadil)
        auto col_expr = std::make_unique<ColumnValueExpression>(0);
        auto const_expr = std::make_unique<ConstantValueExpression>(Value(47));
        auto condition = std::make_unique<EqualExpression>(std::move(col_expr), std::move(const_expr));

        // Updates: Set Id = 99, name = "Aadil_Updated"
        std::vector<std::pair<std::string, Value>> updates = {
            {"name", Value("Aadil_Updated")}};

        assert(cata.UpdateRow("student", updates, std::move(condition)) == true);
        std::cout << "\nPassed Test 5: Updated 'Aadil' (Id=47) to (name='Aadil_Updated')" << std::endl;

        // Verify update
        std::vector<std::string> target_cols = {"Id", "name"};
        auto res = cata.Query("student", target_cols);
        print_table(std::get<1>(res), std::get<2>(res));
    }

    // NEW TEST : SECOND TABLE
    std::vector<Column> cols2 = {
        {const_cast<char *>("Id"), TypeId::INT32, 4, 0, 1},
        {const_cast<char *>("name"), TypeId::VARCHAR, 20, 0, 0},
        {const_cast<char *>("marks"), TypeId::INT32, 4, 0, 0}};
    Schema schema2(cols2);
    {
        catalog cata(filename);
        assert(cata.createTable("table2", schema2) == true);
        auto res = cata.GetDBMeta();
        assert(res.empty() == false);
        std::cout<<"DB meta"<<std::endl;
        print_table(tab_schema, res);
        std::cout<<"Second Table Created"<<std::endl;
    }
    {
        catalog cata(filename);
        std::vector<std::vector<Value>> values = {{Value(1), Value("Awwab"), Value(95)}, {Value(4), Value("Aadil"), Value(83)}, {Value(5), Value("Aribah"), Value(100)},{Value(1), Value("Awab"), Value(100)}};
        int32_t cnt;
        assert(cata.InsertRow("table2", values,&cnt) == true);
        std::cout<<"Inserted "<<cnt<<" rows"<<std::endl;
        std::vector<std::string> target_cols = {"Id", "name", "marks"};
        auto res = cata.Query("table2", target_cols);
        assert(std::get<0>(res) == true);

        std::cout << "\nFetched All Values:" << std::endl;
        print_table(std::get<1>(res), std::get<2>(res));
        std::cout << "\nPassed Test 6: Created second table : table2 and tested insert and read operations" << std::endl;

    }
    // --- NEW TEST: DELETE ROW ---
    {
        catalog cata(filename);

        // Condition: Id == 60 (Awwab)
        auto col_expr = std::make_unique<ColumnValueExpression>(0);
        auto const_expr = std::make_unique<ConstantValueExpression>(Value(60));
        auto condition = std::make_unique<EqualExpression>(std::move(col_expr), std::move(const_expr));

        assert(cata.DeleteRow("student", std::move(condition)) == true);
        std::cout << "\nPassed Test 7: Deleted row where Id == 60 ('Awwab')" << std::endl;

        // Verify deletion
        std::vector<std::string> target_cols = {"Id", "name"};
        auto res = cata.Query("student", target_cols);
        print_table(std::get<1>(res), std::get<2>(res));
    }

    // --- NEW TEST: DELETE TABLE ---
    {
        catalog cata(filename);
        assert(cata.DeleteTable("student") == true);
        std::cout << "\nPassed Test 8: Deleted table 'student'" << std::endl;

        // Verify table no longer exists
        Schema *schem = cata.GetTableSchema("student");
        assert(schem == nullptr);
        std::cout << "Verified table 'student' no longer exists in catalog." << std::endl;
        auto res = cata.GetDBMeta();
        assert(res.empty() == false);
        std::cout<<"Updated DB meta"<<std::endl;
        print_table(tab_schema, res);
    }
    std::cout<<"All tests Passed !"<<std::endl;
    std::remove(filename.c_str());
    return 0;
}