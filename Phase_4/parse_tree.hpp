#ifndef PARSE_TREE_HPP
#define PARSE_TREE_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include "value.hpp"

enum TokenType {
    ROOT,
    BRACKET_VALUES,
    STR,
    COMMA,
    QUOTES,
    STAR,
    NUM
};

class Token {
private:
    std::vector<Token*> children;
    TokenType type_;
    Value value_;

public:
    Token(TokenType type) : type_(type) {}

    // 1. Add destructor to free child pointers and prevent memory leaks
    ~Token() {
        for (Token* child : children) {
            delete child;
        }
    }

    // 2. Prevent copying to avoid double-freeing the pointers in the vector
    Token(const Token&) = delete;
    Token& operator=(const Token&) = delete;

    const TokenType getType() const { return type_; }

    const Value getValue() const {
        try {
            if (type_ == TokenType::BRACKET_VALUES || type_ == TokenType::ROOT || type_ == TokenType::QUOTES) {
                throw std::invalid_argument("Token of type BRACKET_VALUES or ROOT does not have a value");
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
        }
        if (type_ == TokenType::COMMA) return Value(",");
        else if (type_ == TokenType::STAR) return Value("*");
        return value_;
    }

    void setValue(Value value) {
        try {
            if (type_ == TokenType::BRACKET_VALUES || type_ == TokenType::ROOT || type_ == TokenType::QUOTES) {
                throw std::invalid_argument("Token of type BRACKET_VALUES or ROOT does not have a value");
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            return;
        }
        value_ = value;
    }

    // 3. Change parameter to a pointer to explicitly take ownership of heap memory
    void addChild(Token* tok) {
        try {
            // 4. Fix operator precedence
            if (type_ != TokenType::BRACKET_VALUES && type_ != TokenType::ROOT && type_ != TokenType::QUOTES) {
                throw std::invalid_argument("Only Token of type BRACKET_VALUES or ROOT or Quotes have children");
            }
        }
        catch (const std::invalid_argument& e) {
            std::cerr << e.what() << '\n';
            delete tok; // Prevent leak if rejected
            return;
        }
        children.push_back(tok);
    }

    const std::vector<Token*>& getChildren() const { return children; }

    const Token* getChildI(int i) const {
        if(i >= children.size() || i < 0){
            return nullptr;
        }
        return children[i];
    }
};

#endif