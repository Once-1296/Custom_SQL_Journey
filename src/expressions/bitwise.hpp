#ifndef EXPRESSIONS_BITWISE_HPP
#define EXPRESSIONS_BITWISE_HPP

#include <cstdint>
#include <memory>
#include <cassert>
#include <utility>
#include "../abstract_expression.hpp"

// Represents a binary bitwise AND operation
class BitwiseAndExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    BitwiseAndExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for Bitwise AND operation");
        // Perform an addition and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

        int32_t result = left_int & right_int;
        return Value(result);
    }
};

// Represents a bitwise OR operation
class BitwiseOrExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    BitwiseOrExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for Bitwise OR operation");
        // Perform an addition and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

        int32_t result = left_int | right_int;
        return Value(result);
    }
};

// Represents a bitwise XOR operation
class BitwiseXorExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    BitwiseXorExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);

        bool is_valid_expr = true;
        // Ensure both left-hand and right-hand evaluations result in TypeId::INT32.
        assert(lhs.GetType() == rhs.GetType() && lhs.GetType() == TypeId::INT32 && "Unsupported types for Bitwise XOR operation");
        // Perform a multiplication and return the result.
        int32_t left_int = lhs.AsInt32(), right_int = rhs.AsInt32();

        int32_t result = left_int ^ right_int;
        return Value(result);
    }
};

// Represents a bitwise not (flip) operation
class BitwiseNotExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> operand_;
public:
    BitwiseNotExpression(std::unique_ptr<AbstractExpression> operand)
        : operand_(std::move(operand)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value result = operand_->Evaluate(tuple, schema);
        assert(result.GetType() == TypeId::INT32 && "Unsupported types for Bitwise NOT operation");
        int32_t int_value = result.AsInt32();
        return Value(~int_value);
    }
};


#endif
