// abstract_expression.hpp
#ifndef ABSTRACT_EXPRESSION_HPP
#define ABSTRACT_EXPRESSION_HPP
#include <iostream>
#include <cstdint>
#include <memory>
#include "tuple.hpp"
#include "value.hpp"

class AbstractExpression
{
public:
    virtual ~AbstractExpression() = default;

    // Evaluates the expression against a specific physical tuple
    virtual Value Evaluate(const Tuple *tuple, const Schema &schema) const = 0;
};

// Represents a literal constant value inside a query (e.g., the number 21)
class ConstantValueExpression : public AbstractExpression
{
private:
    Value val_;

public:
    explicit ConstantValueExpression(Value val) : val_(val) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        return val_;
    }
};

// Represents a variable reference to a column inside a tuple (e.g., table.age)
class ColumnValueExpression : public AbstractExpression
{
private:
    uint32_t col_idx_;

public:
    explicit ColumnValueExpression(uint32_t col_idx) : col_idx_(col_idx) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        const Column &col = schema.GetColumn(col_idx_);

        // Inspect the target column's TypeId.
        // Extract the matching type variant from the tuple using tuple->GetInt32 or tuple->GetVarchar,
        // and return it wrapped cleanly inside a generic Value object.
        if (col.type == TypeId::INT32)
        {
            return Value(tuple->GetInt32(schema, col_idx_));
        }
        else if (col.type == TypeId::VARCHAR)
        {
            // Write your extraction code here
            return Value(tuple->GetVarchar(schema, col_idx_));
        }

        assert(false && "Unknown TypeId encountered");
        return Value();
    }
};

// Represents a binary comparison operation (For today, we will explicitly implement GT '>')
class ComparisonExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    ComparisonExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32  && "Unsupported types for comparison");
        // Perform a greater-than comparison (lhs > rhs) and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();
        // Hint: Since our engine lacks a BOOLEAN TypeId right now, represent standard SQL truth
        // by returning an INT32 Value where 1 = true and 0 = false.

        // Write your comparison logic here
        int32_t result = (left_int > right_int) ? 1 : 0;
        return Value(result);
    }
};

#endif