// value.hpp
#ifndef VALUE_HPP
#define VALUE_HPP

#include <string>
#include <cstdint>
#include <cassert>
#include "schema.hpp"

class Value {
private:
    TypeId type_;
    union {
        int32_t int32_val;
    };
    std::string varchar_val; // Handled separately due to non-trivial constructor

public:
    Value() : type_(TypeId::INT32), int32_val(0) {}
    Value(int32_t val) : type_(TypeId::INT32), int32_val(val) {}
    Value(std::string val) : type_(TypeId::VARCHAR), varchar_val(val) {}

    TypeId GetType() const { return type_; }
    int32_t AsInt32() const { assert(type_ == TypeId::INT32); return int32_val; }
    std::string AsVarchar() const { assert(type_ == TypeId::VARCHAR); return varchar_val; }
};
#endif