#ifndef INSERT_PLAN_HPP
#define INSERT_PLAN_HPP

#include <string>
#include <memory>
#include "../abstract_plan.hpp" // Assumes your base plan node header

class InsertPlanNode
{
public:
    std::string table_name_;

    InsertPlanNode(const std::string &table_name)
        : table_name_(table_name) {}

    const std::string GetTableName() const { return table_name_; }
};

#endif