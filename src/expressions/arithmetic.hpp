#ifndef EXPRESSIONS_ARITHMETIC_HPP
#define EXPRESSIONS_ARITHMETIC_HPP

#include <cstdint>
#include <memory>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <utility>
#include "../abstract_expression.hpp"

// Represents a binary addition operation
class AdditionExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    AdditionExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for addition");
        // Perform an addition and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

        int64_t result = static_cast<int64_t>(left_int) + right_int;
        if (result > std::numeric_limits<int32_t>::max())
            throw std::overflow_error("integer addition overflow");
        if (result < std::numeric_limits<int32_t>::min())
            throw std::underflow_error("integer addition underflow");
        return Value(static_cast<int32_t>(result));
    }
};

// Represents a binary subtraction operation
class SubtractionExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    SubtractionExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for subtraction");
        // Perform an addition and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

        int64_t result = static_cast<int64_t>(left_int) - right_int;
        if (result > std::numeric_limits<int32_t>::max())
            throw std::overflow_error("integer subtraction overflow");
        if (result < std::numeric_limits<int32_t>::min())
            throw std::underflow_error("integer subtraction underflow");
        return Value(static_cast<int32_t>(result));
    }
};

// Represents a binary multiplication operation
class MultiplyExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    MultiplyExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for multiplication");
        // Perform a multiplication and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

        int64_t result = static_cast<int64_t>(left_int) * right_int;
        if (result > std::numeric_limits<int32_t>::max())
            throw std::overflow_error("integer multiplication overflow");
        if (result < std::numeric_limits<int32_t>::min())
            throw std::underflow_error("integer multiplication underflow");
        return Value(static_cast<int32_t>(result));
    }
};

// Represents a binary division operation
class DivisionExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    DivisionExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for division");
        // Perform an division and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

        if (right_int == 0)
            throw std::invalid_argument("division by zero");
        if (left_int == std::numeric_limits<int32_t>::min() && right_int == -1)
            throw std::overflow_error("integer division overflow");
        return Value(left_int / right_int);
    }
};

// Represents a negation
class NegationExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> operand_;
public:
    NegationExpression(std::unique_ptr<AbstractExpression> operand)
        : operand_(std::move(operand)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value result = operand_->Evaluate(tuple, schema);
        assert(result.GetType() == TypeId::INT32 && "Unsupported types for negation operation");
        int32_t int_value = result.AsInt32();
        if (int_value == std::numeric_limits<int32_t>::min())
            throw std::overflow_error("integer negation overflow");
        return Value(-int_value);
    }
}; 

#endif
