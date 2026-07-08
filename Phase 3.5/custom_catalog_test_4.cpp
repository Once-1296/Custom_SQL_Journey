#include <iostream>
#include <iomanip>
#include <vector>
#include "custom_catalog.hpp"

// Helper function to print tuples in a formatted SQL-like table
void print_table(const Schema& schema, const std::vector<Tuple>& vec) {
    uint32_t n = schema.GetColumnCount();
    if (n == 0) return;

    // Print top border
    std::cout << "+";
    for (uint32_t i = 0; i < n; i++) std::cout << std::string(17, '-') << "+";
    std::cout << "\n|";
    
    // Print column headers
    for (uint32_t i = 0; i < n; i++) {
        std::cout << " " << std::left << std::setw(15) << schema.GetColumn(i).name << " |";
    }
    
    // Print separator
    std::cout << "\n+";
    for (uint32_t i = 0; i < n; i++) std::cout << std::string(17, '-') << "+";
    std::cout << "\n";

    // Print rows
    for (const auto& tuple : vec) {
        std::cout << "|";
        for (uint32_t i = 0; i < n; i++) {
            const Column& col = schema.GetColumn(i);
            if (col.type == TypeId::INT32) {
                std::cout << " " << std::left << std::setw(15) << tuple.GetInt32(schema, i) << " |";
            } else {
                std::cout << " " << std::left << std::setw(15) << tuple.GetVarchar(schema, i) << " |";
            }
        }
        std::cout << "\n";
    }

    // Print bottom border
    std::cout << "+";
    for (uint32_t i = 0; i < n; i++) std::cout << std::string(17, '-') << "+";
    std::cout << "\n";
}

int main()
{
    std::string filename = "test_4.db";
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
        std::vector<Value> values = {Value(60), Value("Awwab")};
        assert(cata.InsertRow("student", values) == true);
        
        assert(cata.InsertRow("nonexist", values) == false);
        
        values = {Value("19"), Value("Awwab")}; // Invalid type (String for INT32)
        assert(cata.InsertRow("student", values) == false);
        
        values = {Value(47), Value("Aadil")};
        assert(cata.InsertRow("student", values) == true);
        
        values = {Value(50), Value("Aribah")};
        assert(cata.InsertRow("student", values) == true);
        
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
            {"Id", Value(99)}, 
            {"name", Value("Aadil_Updated")}
        };
        
        assert(cata.UpdateRow("student", updates, std::move(condition)) == true);
        std::cout << "\nPassed Test 5: Updated 'Aadil' (Id=47) to (Id=99, name='Aadil_Updated')" << std::endl;

        // Verify update
        std::vector<std::string> target_cols = {"Id", "name"};
        auto res = cata.Query("student", target_cols);
        print_table(std::get<1>(res), std::get<2>(res));
    }

    // --- NEW TEST: DELETE ROW ---
    {
        catalog cata(filename);
        
        // Condition: Id == 60 (Awwab)
        auto col_expr = std::make_unique<ColumnValueExpression>(0); 
        auto const_expr = std::make_unique<ConstantValueExpression>(Value(60));
        auto condition = std::make_unique<EqualExpression>(std::move(col_expr), std::move(const_expr));
        
        assert(cata.DeleteRow("student", std::move(condition)) == true);
        std::cout << "\nPassed Test 6: Deleted row where Id == 60 ('Awwab')" << std::endl;

        // Verify deletion
        std::vector<std::string> target_cols = {"Id", "name"};
        auto res = cata.Query("student", target_cols);
        print_table(std::get<1>(res), std::get<2>(res));
    }

    // --- NEW TEST: DELETE TABLE ---
    {
        catalog cata(filename);
        assert(cata.DeleteTable("student") == true);
        std::cout << "\nPassed Test 7: Deleted table 'student'" << std::endl;
        
        // Verify table no longer exists
        Schema* schem = cata.GetTableSchema("student");
        assert(schem == nullptr);
        std::cout << "Verified table 'student' no longer exists in catalog." << std::endl;
    }

    std::remove(filename.c_str());
    return 0;
}