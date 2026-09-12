#ifndef EXPRESSIONS_STRING_EXPRESSIONS_HPP
#define EXPRESSIONS_STRING_EXPRESSIONS_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <cassert>
#include <utility>
#include "../abstract_expression.hpp"

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
        // is right_str a prefix of left_str?
        uint32_t l_n = left_str.size(), r_n = right_str.size(), i =0;
        for( ;i < l_n && i<r_n;i++)
        {
            flag = left_str[i] == right_str[i];
            if(!flag)break;
        }
        flag = i == r_n;
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
        // is right_str a suffix of left_str?
        int32_t l_n = left_str.size(), r_n = right_str.size(), i =l_n-1, j= r_n - 1;
        for( ;i >= 0 && j>=0;i--,j--)
        {
            flag = left_str[i] == right_str[j];
            if(!flag)break;
        }
        flag = j < 0;
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

#endif
