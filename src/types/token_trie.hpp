#ifndef TOKEN_TRIE_HPP
#define TOKEN_TRIE_HPP

#include <iostream>
#include <optional>
#include <vector>
#include <cassert>
#include "Token.hpp"

class tokenTrie {
private:
    tokenTrie* children[26];
    std::optional<Token> value;

public:
    tokenTrie() {
        for (int32_t i = 0; i < 26; i++) children[i] = nullptr;
    }
    
    tokenTrie(Token s) {
        for (int32_t i = 0; i < 26; i++) children[i] = nullptr;
        value = s;
    }
    
    ~tokenTrie() {
        for (int32_t i = 0; i < 26; i++) {
            if (children[i] != nullptr) {
                delete children[i];
            }
        }
    }
    
    void insert(const char* keyword, Token& token) {
        if (*keyword == '\0') {   
            this->value = token;
            return;
        }
        int32_t i = *keyword - 'A';
        assert(i >= 0 && i < 26);
        if (this->children[i] == nullptr) {
            this->children[i] = new tokenTrie();
        }
        this->children[i]->insert(keyword + 1, token);
    }
    
    const tokenTrie* getChild(int32_t i) const {
        if (i < 0 || i >= 26) return nullptr;
        return children[i];
    }
    
    const std::optional<Token> getValue() const {
        return value;
    }
};
#endif