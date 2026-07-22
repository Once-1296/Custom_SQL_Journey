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

// Represents a binary comparison operation 
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
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for comparison");
        // Perform a greater-than comparison (lhs > rhs) and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();
        // Hint: Since our engine lacks a BOOLEAN TypeId right now, represent standard SQL truth
        // by returning an INT32 Value where 1 = true and 0 = false.

        // Write your comparison logic here
        int32_t result = (left_int > right_int) ? 1 : 0;
        return Value(result);
    }
};

// Represents a binary comparison operation 
class ComparisonLesserExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    ComparisonLesserExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for comparison");
        // Perform a lesser-than comparison (lhs < rhs) and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();
        // Hint: Since our engine lacks a BOOLEAN TypeId right now, represent standard SQL truth
        // by returning an INT32 Value where 1 = true and 0 = false.

        // Write your comparison logic here
        int32_t result = (left_int < right_int) ? 1 : 0;
        return Value(result);
    }
};

class TruthyExpression : public AbstractExpression
{
private:
    Value val_;

public:
    TruthyExpression(Value val) : val_(val) {}

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

class EqualExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    EqualExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);
        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && "Unsupported types for comparison");
        // Perform a equal to comparison (lhs == rhs) and return the result.
        if (lhs.GetType() == TypeId::INT32)
        {
            int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

            // Write your comparison logic here
            int32_t result = (left_int == right_int) ? 1 : 0;
            return Value(result);
        }
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        // Write your comparison logic here
        int32_t result = (left_str == right_str) ? 1 : 0;
        return Value(result);
    }
};

class NotEqualExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    NotEqualExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);
        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && "Unsupported types for comparison");
        // Perform a not equal (lhs != rhs) comparison and return the result.
        if (lhs.GetType() == TypeId::INT32)
        {
            int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

            // Write your comparison logic here
            int32_t result = (left_int != right_int) ? 1 : 0;
            return Value(result);
        }
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        // Write your comparison logic here
        int32_t result = (left_str != right_str) ? 1 : 0;
        return Value(result);
    }
};

// Represents a prefix match
class PrefixMatchExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    PrefixMatchExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::VARCHAR.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::VARCHAR && "Unsupported types for comparison");
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        bool flag = 1;
        // Write your comparison logic here
        // is left_str a prefix of right_str?
        uint32_t l_n = left_str.size(), r_n = right_str.size(), i =0;
        for( ;i < l_n && i<r_n;i++)
        {
            flag = left_str[i] == right_str[i];
            if(!flag)break;
        }
        flag = i == l_n;
        int32_t result = (flag) ? 1 : 0;
        return Value(result);
    }
};

// Represents a suffix match
class SuffixMatchExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    SuffixMatchExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::VARCHAR.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::VARCHAR && "Unsupported types for comparison");
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        bool flag = 1;
        // Write your comparison logic here
        // is left_str a suffix of right_str?
        uint32_t l_n = left_str.size(), r_n = right_str.size(), i =l_n-1, j= r_n - 1;
        for( ;i >= 0 && j>=0;i--,j--)
        {
            flag = left_str[i] == right_str[j];
            if(!flag)break;
        }
        flag = i < 0;
        int32_t result = (flag) ? 1 : 0;
        return Value(result);
    }
};

// Represents a subsequence match
class SubSeqMatchExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    SubSeqMatchExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::VARCHAR.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::VARCHAR && "Unsupported types for comparison");
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        bool flag = 1;
        // Write your comparison logic here
        // is left_str a subsequence of right_str?
        uint32_t l_n = left_str.size(), r_n = right_str.size(), i =0, j= 0;
        for( ;i < l_n && j < r_n;j++)
        {
            if(left_str[i] == right_str[j])
            {
                i++;
            }
        }
        flag = i == l_n;
        int32_t result = (flag) ? 1 : 0;
        return Value(result);
    }
};

class CNFExpression : public AbstractExpression
{
private:
    std::vector<std::vector<std::unique_ptr<AbstractExpression>>> expressions_;

public:
    CNFExpression(std::vector<std::vector<std::unique_ptr<AbstractExpression>>> expressions) : expressions_(std::move(expressions)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        // for each row, at least one evaluates to true
        // if any row is false, fail
        uint32_t n = expressions_.size();
        for (uint32_t i = 0; i < n; i++)
        {
            uint32_t m = expressions_[i].size();
            bool row_result = false;
            for (uint32_t j = 0; j < m; j++)
            {
                Value val = expressions_[i][j]->Evaluate(tuple, schema);
                assert(val.GetType() == TypeId::INT32 && "Unsupported types for comparison");
                if (val.AsInt32() == 1)
                {
                    row_result = true;
                    break;
                }
            }
            if (!row_result)
            {
                return Value(0);
            }
        }
        return Value(1);
    }
};
#endif