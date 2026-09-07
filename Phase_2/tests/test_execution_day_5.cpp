// test_day5.cpp
#include "../src/types/schema.hpp"
#include "../src/types/tuple.hpp"
#include "../src/types/value.hpp"
#include "../src/abstract_expression.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <array>

void TestExpressionTree() {
    std::vector<Column> cols = {
        {"id",  TypeId::INT32, 4, 0},
        {"age", TypeId::INT32, 4, 0}
    };
    Schema schema(cols);

    // Mock a tuple's memory layout: ID = 1, Age = 25
    std::array<uint8_t, 8> mock_bytes{0};
    int32_t id = 1; int32_t age = 25;
    std::memcpy(mock_bytes.data() + 0, &id, 4);
    std::memcpy(mock_bytes.data() + 4, &age, 4);
    Tuple tuple(mock_bytes.data(), 8, RID(0,0));

    // Construct expression tree representing: (age > 21)
    auto col_expr = std::make_unique<ColumnValueExpression>(1); // column 1 is 'age'
    auto const_expr = std::make_unique<ConstantValueExpression>(Value(21));
    ComparisonExpression comp_expr(std::move(col_expr), std::move(const_expr));

    // Evaluate expression tree on our tuple
    Value result = comp_expr.Evaluate(&tuple, schema);

    // Assert that 25 > 21 yields True (1)
    assert(result.AsInt32() == 1);

    // Construct evaluation tree representing: (age > 30)
    auto col_expr_2 = std::make_unique<ColumnValueExpression>(1);
    auto const_expr_2 = std::make_unique<ConstantValueExpression>(Value(30));
    ComparisonExpression comp_expr_2(std::move(col_expr_2), std::move(const_expr_2));

    // Evaluate second tree on identical tuple data
    Value result_2 = comp_expr_2.Evaluate(&tuple, schema);

    // Assert that 25 > 30 yields False (0)
    assert(result_2.AsInt32() == 0);

    std::cout << "Day 5 Expression Infrastructure Tree Tests Passed!" << std::endl;
}

int main() {
    TestExpressionTree();
    return 0;
}