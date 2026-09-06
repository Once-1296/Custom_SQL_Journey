// abstract_expression.hpp
#ifndef ABSTRACT_EXPRESSION_HPP
#define ABSTRACT_EXPRESSION_HPP
#include <cstdint>
#include <memory>
#include <utility>
#include "types/tuple.hpp"
#include "types/value.hpp"

class AbstractExpression
{
public:
    virtual ~AbstractExpression() = default;

    // Evaluates the expression against a specific physical tuple
    virtual Value Evaluate(const Tuple *tuple, const Schema &schema) const = 0;
};
#include "expressions/literals.hpp"
#include "expressions/comparisons.hpp"
#include "expressions/arithmetic.hpp"
#include "expressions/string_expressions.hpp"
#include "expressions/cnf.hpp"
#endif
