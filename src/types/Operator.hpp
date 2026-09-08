#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "schema.hpp"
#include <vector>
class BinaryOperator{
    private:
    std::vector<std::pair<TypeId, TypeId>> validPairs;
    TypeId resultType;
    public:
    BinaryOperator(){
        validPairs = {{TypeId::INT32, TypeId::INT32}, {}}; // all pairs;
        resultType = TypeId::INT32; // default
    }
    BinaryOperator(std::vector<std::pair<TypeId, TypeId>> validPairs, TypeId resultType){
        this->validPairs = validPairs;
        this->resultType = resultType;
    }
    bool isValidPair(TypeId left, TypeId right){
        for(auto &pair: validPairs){
            if(pair.first == left && pair.second == right){
                return true;
            }
        }
        return false;
    }
    const TypeId getResultType() const{
        return resultType;
    }
    const std::vector<std::pair<TypeId, TypeId>>& getValidPairs() const{
        return validPairs;
    }
};

class UnaryOperator{
    private:
    std::vector<TypeId> validTypes;
    TypeId resultType;
    public:
    UnaryOperator(){
        validTypes = {TypeId::INT32, TypeId::VARCHAR}; // all types
        resultType = TypeId::INT32; // default
    }
    UnaryOperator(std::vector<TypeId> validTypes, TypeId resultType){
        this->validTypes = validTypes;
        this->resultType = resultType;
    }
    bool isValidType(TypeId type){
        for(auto &t: validTypes){
            if(t == type){
                return true;
            }
        }
        return false;
    }
    const TypeId getResultType() const{
        return resultType;
    }
    const std::vector<TypeId>& getValidTypes() const{
        return validTypes;
    }
};

#endif