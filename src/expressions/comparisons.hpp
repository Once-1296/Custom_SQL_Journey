#ifndef EXPRESSIONS_COMPARISONS_HPP
#define EXPRESSIONS_COMPARISONS_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <cassert>
#include <utility>
#include "../abstract_expression.hpp"

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
        // Ensure both left-hand and right-hand evaluations result in same type.
        assert(lhs.GetType() == rhs.GetType() && "Unsupported types for comparison");
        // Perform a greater than comparison (lhs > rhs) and return the result.
        if (lhs.GetType() == TypeId::INT32)
        {
            int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

            // Write your comparison logic here
            int32_t result = (left_int > right_int) ? 1 : 0;
            return Value(result);
        }
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        // Write your comparison logic here
        int32_t result = (left_str > right_str) ? 1 : 0;
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
        // Ensure both left-hand and right-hand evaluations result in same type.
        assert(lhs.GetType() == rhs.GetType() && "Unsupported types for comparison");
        // Perform a lesser than comparison (lhs < rhs) and return the result.
        if (lhs.GetType() == TypeId::INT32)
        {
            int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

            // Write your comparison logic here
            int32_t result = (left_int < right_int) ? 1 : 0;
            return Value(result);
        }
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        // Write your comparison logic here
        int32_t result = (left_str < right_str) ? 1 : 0;
        return Value(result);
    }
};

// Represents a binary comparison operation
class LesserOrEqualExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    LesserOrEqualExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in same type.
        assert(lhs.GetType() == rhs.GetType() && "Unsupported types for comparison");
        // Perform a lesser than or equal to comparison (lhs <= rhs) and return the result.
        if (lhs.GetType() == TypeId::INT32)
        {
            int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

            // Write your comparison logic here
            int32_t result = (left_int <= right_int) ? 1 : 0;
            return Value(result);
        }
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        // Write your comparison logic here
        int32_t result = (left_str <= right_str) ? 1 : 0;
        return Value(result);
    }
};

// Represents a binary comparison operation
class GreaterOrEqualExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    GreaterOrEqualExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in same type.
        assert(lhs.GetType() == rhs.GetType() && "Unsupported types for comparison");
        // Perform a greater than or equal to comparison (lhs >= rhs) and return the result.
        if (lhs.GetType() == TypeId::INT32)
        {
            int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

            // Write your comparison logic here
            int32_t result = (left_int >= right_int) ? 1 : 0;
            return Value(result);
        }
        std::string left_str = lhs.AsVarchar(), right_str = rhs.AsVarchar();
        // Write your comparison logic here
        int32_t result = (left_str >= right_str) ? 1 : 0;
        return Value(result);
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

#endif
