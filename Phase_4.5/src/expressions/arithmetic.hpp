#ifndef EXPRESSIONS_ARITHMETIC_HPP
#define EXPRESSIONS_ARITHMETIC_HPP

#include <cstdint>
#include <memory>
#include <cassert>
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

        // TODO: Handle overflow
        int32_t result = left_int + right_int;
        return Value(result);
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

        // TODO: Handle overflow
        int32_t result = left_int - right_int;
        return Value(result);
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

        // TODO: Handle overflow
        int32_t result = left_int * right_int;
        return Value(result);
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

        // TODO: Handle overflow
        int32_t result = left_int / right_int;
        return Value(result);
    }
};

#endif
