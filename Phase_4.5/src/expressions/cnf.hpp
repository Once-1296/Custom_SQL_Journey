#ifndef EXPRESSIONS_CNF_HPP
#define EXPRESSIONS_CNF_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <cassert>
#include <utility>
#include "../abstract_expression.hpp"

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
