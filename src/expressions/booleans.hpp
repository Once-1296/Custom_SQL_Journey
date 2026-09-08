#ifndef EXPRESSIONS_BOOLEANS_HPP
#define EXPRESSIONS_BOOLEANS_HPP

#include <cstdint>
#include <cassert>
#include <memory>
#include <utility>
#include "../abstract_expression.hpp"
#include "literals.hpp"


// Represents a logical AND operation
class ANDExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    ANDExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);
        Value truthy_left = TruthyExpression(lhs).Evaluate(tuple, schema);
        Value truthy_right = TruthyExpression(rhs).Evaluate(tuple, schema);
        // verify both truthy values are int
        assert(truthy_left.GetType() == TypeId::INT32 && truthy_right.GetType() == TypeId::INT32 && "Unsupported types for AND operation");
        int32_t result = (truthy_left.AsInt32() && truthy_right.AsInt32()) ? 1 : 0;
        return Value(result);
    }
};

// Represents a logical OR operation
class ORExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    ORExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);
        Value truthy_left = TruthyExpression(lhs).Evaluate(tuple, schema);
        Value truthy_right = TruthyExpression(rhs).Evaluate(tuple, schema);
        // verify both truthy values are int
        assert(truthy_left.GetType() == TypeId::INT32 && truthy_right.GetType() == TypeId::INT32 && "Unsupported types for OR operation");
        int32_t result = (truthy_left.AsInt32() || truthy_right.AsInt32()) ? 1 : 0;
        return Value(result);
    }
};

// Represents a logical NOT operation
class NOTExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> operand_;

public:
    NOTExpression(std::unique_ptr<AbstractExpression> operand)
        : operand_(std::move(operand)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value operand = operand_->Evaluate(tuple, schema);
        Value truthy_operand = TruthyExpression(operand).Evaluate(tuple, schema);
        // verify truthy value is int
        assert(truthy_operand.GetType() == TypeId::INT32 && "Unsupported types for NOT operation");
        int32_t result = (truthy_operand.AsInt32() == 0) ? 1 : 0;
        return Value(result);
    }
};

// Represents a logical XOR operation
class XORExpression : public AbstractExpression
{
private:
    std::unique_ptr<AbstractExpression> left_;
    std::unique_ptr<AbstractExpression> right_;

public:
    XORExpression(std::unique_ptr<AbstractExpression> left, std::unique_ptr<AbstractExpression> right)
        : left_(std::move(left)), right_(std::move(right)) {}

    Value Evaluate(const Tuple *tuple, const Schema &schema) const override
    {
        Value lhs = left_->Evaluate(tuple, schema);
        Value rhs = right_->Evaluate(tuple, schema);
        Value truthy_left = TruthyExpression(lhs).Evaluate(tuple, schema);
        Value truthy_right = TruthyExpression(rhs).Evaluate(tuple, schema);
        // verify both truthy values are int
        assert(truthy_left.GetType() == TypeId::INT32 && truthy_right.GetType() == TypeId::INT32 && "Unsupported types for XOR operation");
        int32_t result = (truthy_left.AsInt32() ^ truthy_right.AsInt32()) ? 1 : 0;
        return Value(result);
    }
};



#endif