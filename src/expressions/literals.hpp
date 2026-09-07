#ifndef EXPRESSIONS_LITERALS_HPP
#define EXPRESSIONS_LITERALS_HPP

#include <cstdint>
#include <cassert>
#include <memory>
#include <utility>
#include "../abstract_expression.hpp"

// Represents a literal constant value inside a query (e.g., the number 21)
class ConstantValueExpression : public AbstractExpression
{
private:
    Value val_;

public:
    explicit ConstantValueExpression(Value val) : val_(std::move(val)) {}

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

class TruthyExpression : public AbstractExpression
{
private:
    Value val_;

public:
    TruthyExpression(Value val) : val_(std::move(val)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        uint32_t flag = true;
        if (val_.GetType() == TypeId::INT32)
        {
            flag = (Value(0) != val_);
        }
        else if (val_.GetType() == TypeId::VARCHAR)
        {
            flag = (Value("") != val_);
        }
        return Value(flag);
    }
};

#endif
