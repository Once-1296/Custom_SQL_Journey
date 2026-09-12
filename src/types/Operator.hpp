#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "schema.hpp"
#include "../abstract_expression.hpp"
#include "value.hpp"
#include "Token.hpp"
#include <vector>
#include <map>
#include <stack>
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

uint32_t getPriority(std::string &op)
{
    if(op == "(")
    {
        return 0;
    }
    if(op == "=" || op == "!=" || op == ">" || op == "<" || op == ">=" || op == "<=")
    {
        return 1;
    }
    if(op == "AND" || op == "OR" || op == "XOR")
    {
        return 2;
    }
    if(op == "NOT" || op == "!")
    {
        return 3;
    }
    if(op == "&" || op == "^" || op == "|"|| op == "~")
    {
        return 4;
    }
    if(op == "+" || op == "-" || op == "*" || op == "/" || op == "SUB")
    {
        return 5;
    }
    if(op == "STARTSWITH" || op == "ENDSWITH")
    {
        return 6;
    }
    return -1;
}


std::map<std::string, UnaryOperator> unOpMap = {
    {"-", UnaryOperator({TypeId::INT32}, TypeId::INT32)},
    {"~", UnaryOperator({TypeId::INT32}, TypeId::INT32)},
    {"NOT", UnaryOperator({TypeId::INT32, TypeId::VARCHAR}, TypeId::INT32)},
    {"!", UnaryOperator({TypeId::INT32, TypeId::VARCHAR}, TypeId::INT32)}
};
std::map<std::string, BinaryOperator> binOpMap = {
    {"+", BinaryOperator({{TypeId::INT32, TypeId::INT32}}, TypeId::INT32)},
    {"SUB", BinaryOperator({{TypeId::INT32, TypeId::INT32}}, TypeId::INT32)},
    {"*", BinaryOperator({{TypeId::INT32, TypeId::INT32}}, TypeId::INT32)},
    {"/", BinaryOperator({{TypeId::INT32, TypeId::INT32}}, TypeId::INT32)},
    {"&", BinaryOperator({{TypeId::INT32, TypeId::INT32}}, TypeId::INT32)},
    {"|", BinaryOperator({{TypeId::INT32, TypeId::INT32}}, TypeId::INT32)},
    {"^", BinaryOperator({{TypeId::INT32, TypeId::INT32}}, TypeId::INT32)},
    {"STARTSWITH", BinaryOperator({{TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {"ENDSWITH", BinaryOperator({{TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {">", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {"<", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {"=", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {"!=", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {">=", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {"<=", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR}}, TypeId::INT32)},
    {"AND", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR},{TypeId::VARCHAR, TypeId::INT32},{TypeId::INT32, TypeId::VARCHAR} }, TypeId::INT32)},
    {"OR", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR},{TypeId::VARCHAR, TypeId::INT32},{TypeId::INT32, TypeId::VARCHAR} }, TypeId::INT32)},
    {"XOR", BinaryOperator({{TypeId::INT32, TypeId::INT32}, {TypeId::VARCHAR, TypeId::VARCHAR},{TypeId::VARCHAR, TypeId::INT32},{TypeId::INT32, TypeId::VARCHAR} }, TypeId::INT32)},
};

std::unique_ptr<AbstractExpression> processUnary(std::unique_ptr<AbstractExpression> uptr, std::string &op)
{
    if(op == "-")
    {
        return std::move(std::make_unique<NegationExpression>(NegationExpression(std::move(uptr))));
    }
    else if(op == "~")
    {
        return std::move(std::make_unique<BitwiseNotExpression>(BitwiseNotExpression(std::move(uptr))));
    }
    else if(op == "NOT" || op == "!")
    {
        return std::move(std::make_unique<NOTExpression>(NOTExpression(std::move(uptr))));
    }
    return nullptr;
}
std::unique_ptr<AbstractExpression> processBinary(std::unique_ptr<AbstractExpression> uptr1, std::unique_ptr<AbstractExpression> uptr2, std::string &op)
{
    if(op == "+")
    {
        return std::move(std::make_unique<AdditionExpression>(AdditionExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "SUB")
    {
        return std::move(std::make_unique<SubtractionExpression>(SubtractionExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "*")
    {
        return std::move(std::make_unique<MultiplyExpression>(MultiplyExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "/")
    {
        return std::move(std::make_unique<DivisionExpression>(DivisionExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "&")
    {
        return std::move(std::make_unique<BitwiseAndExpression>(BitwiseAndExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "|")
    {
        return std::move(std::make_unique<BitwiseOrExpression>(BitwiseOrExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "^")
    {
        return std::move(std::make_unique<BitwiseXorExpression>(BitwiseXorExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "STARTSWITH")
    {
        return std::move(std::make_unique<PrefixMatchExpression>(PrefixMatchExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "ENDSWITH")
    {
        return std::move(std::make_unique<SuffixMatchExpression>(SuffixMatchExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == ">")
    {
        return std::move(std::make_unique<ComparisonExpression>(ComparisonExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "<")
    {
        return std::move(std::make_unique<ComparisonLesserExpression>(ComparisonLesserExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "=")
    {
        return std::move(std::make_unique<EqualExpression>(EqualExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "!=")
    {
        return std::move(std::make_unique<NotEqualExpression>(NotEqualExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == ">=")
    {
        return std::move(std::make_unique<GreaterOrEqualExpression>(GreaterOrEqualExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "<=")
    {
        return std::move(std::make_unique<LesserOrEqualExpression>(LesserOrEqualExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "AND")
    {
        return std::move(std::make_unique<ANDExpression>(ANDExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "OR")
    {
        return std::move(std::make_unique<ORExpression>(ORExpression(std::move(uptr1), std::move(uptr2))));
    }
    else if(op == "XOR")
    {
        return std::move(std::make_unique<XORExpression>(XORExpression(std::move(uptr1), std::move(uptr2))));
    }
    
    return nullptr;
}

bool processOP(std::stack<std::pair<std::unique_ptr<AbstractExpression>, TypeId>>&mainSTK, std::string &op, std::string &Message)
{
    if(mainSTK.empty())
    {
        Message = "EMPTY STACK.";
        return false;
    }
    if(unOpMap.contains(op))
    {
        UnaryOperator unOp = unOpMap[op];
        if(!unOp.isValidType(mainSTK.top().second))
        {
            Message = "Invalid types in WHERE Clause.";
            return false;
        }
        TypeId resType = unOp.getResultType();
        std::unique_ptr<AbstractExpression> uptr = std::move(mainSTK.top().first);
        mainSTK.pop();
        std::unique_ptr<AbstractExpression> nuptr = std::move(processUnary(std::move(uptr),op));
        if(nuptr == nullptr)
        {
            Message = "Invalid types in WHERE Clause.";
            return false;
        }
        mainSTK.push({std::move(nuptr),resType});
        return true;
    }
    if(mainSTK.size() < 2)
    {
        Message = "NOT ENOUGH TOKENS IN STACK.";
        return false;
    }
    if(binOpMap.contains(op))
    {
        BinaryOperator binOp = binOpMap[op];
        std::unique_ptr<AbstractExpression> uptr2 = std::move(mainSTK.top().first);
        TypeId typ2 = mainSTK.top().second;
        mainSTK.pop();
        std::unique_ptr<AbstractExpression> uptr1 = std::move(mainSTK.top().first);
        TypeId typ1 = mainSTK.top().second;
        mainSTK.pop();
        if(!binOp.isValidPair(typ1, typ2))
        {
            Message = "Invalid types in WHERE Clause.";
            return false;
        }
        TypeId resType = binOp.getResultType();
        std::unique_ptr<AbstractExpression> nuptr = std::move(processBinary(std::move(uptr1), std::move(uptr2),op));
        if(nuptr == nullptr)
        {
            Message = "Invalid types in WHERE Clause.";
            return false;
        }
        mainSTK.push({std::move(nuptr),resType});
        return true;
    }
    Message = "Unidentified operator.";
    return false;
}

#endif