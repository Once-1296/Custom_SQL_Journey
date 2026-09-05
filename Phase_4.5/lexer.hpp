#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include "Token.hpp"
#include "token_trie.hpp"

class lexer
{
private:
    tokenTrie *root;

    bool isCaps(char c) { return c <= 'Z' && c >= 'A'; }
    bool isAlpha(char c) { return (c <= 'z' && c >= 'a') || (c >= 'A' && c <= 'Z'); }
    bool isNum(char c) { return (c >= '0' && c <= '9'); }
    bool isUnderScore(char c) { return (c == '_'); }
    bool isAlphaNumUS(char c) { return isAlpha(c) || isNum(c) || isUnderScore(c); }

    bool isOperator(char c)
    {
        char ops[] = {'+', '-', '*', '/', '>', '<', '=', '!'};
        for (auto &o : ops)
            if (c == o)
                return true;
        return false;
    }

public:
    lexer()
    {
        root = new tokenTrie();
        for (auto &keyword : KEYWORDS)
        {
            Token token = {tokenType::KEYWORD, keyword};
            root->insert(keyword, token);
        }
    }

    ~lexer() { delete root; }

    // Print Function Included Directly in Lexer
    void printTokens(const std::vector<Token> &tokens)
    {
        for (const auto &tok : tokens)
        {
            if (tok.value.index() == 0)
                std::cout << std::get<0>(tok.value);
            else
                std::cout << std::get<1>(tok.value);

            std::cout << " : ";
            switch (tok.type)
            {
            case STR:
                std::cout << "STR";
                break;
            case INT:
                std::cout << "INT";
                break;
            case KEYWORD:
                std::cout << "KEYWORD";
                break;
            case OPERATOR:
                std::cout << "OPERATOR";
                break;
            case BRACKET_OPEN:
                std::cout << "BRACKET_OPEN";
                break;
            case BRACKET_CLOSE:
                std::cout << "BRACKET_CLOSE";
                break;
            case COMMA:
                std::cout << "COMMA";
                break;
            }
            std::cout << '\n';
        }
    }

    // Pass query by value to safely modify without destroying user's original string
    bool lex(std::string query, std::vector<Token> &tokens, std::string &Message)
    {
        if(!tokens.empty()){
            tokens.clear();
        }
        bool error = false;
        while (!query.empty() && query.back() == ' ')
            query.pop_back();

        if (query.empty() || query.front() == ';' || query.back() != ';')
        {
            Message = "Query is blank/ does not end with ;";
            return false;
        }

        std::stack<std::pair<char, const tokenTrie *>> st;
        bool isInQuotes = false;
        int bracketCount = 0;

        auto push = [&tokens, &st](bool forceString = false) -> void
        {
            if (st.empty())
                return;

            tokenType type = tokenType::INT;
            bool isint = true;

            // Check trie value *before* emptying stack
            if (!forceString && st.top().second != nullptr)
            {
                std::optional<Token> value = st.top().second->getValue();
                if (value.has_value())
                    type = value.value().type;
            }

            std::string true_s;
            while (!st.empty())
            {
                true_s.push_back(st.top().first);
                isint = isint && (st.top().first >= '0' && st.top().first <= '9');
                st.pop();
            }
            std::reverse(true_s.begin(), true_s.end());

            if (forceString)
            {
                tokens.push_back({tokenType::STR, true_s});
                return;
            }

            if (!isint && type == tokenType::INT)
                type = tokenType::STR;

            for (auto &op : OPERATORS)
            {
                if (true_s == op)
                {
                    type = tokenType::OPERATOR;
                    break;
                }
            }

            if (isint)
                tokens.push_back({type, std::stoi(true_s)});
            else
                tokens.push_back({type, true_s});
        };

        for (auto &c : query)
        {
            if (isInQuotes)
            {
                if (c == '"')
                {
                    push(true); // Force quote contents to be STR
                    isInQuotes = false;
                }
                else
                {
                    st.push({c, nullptr}); // Anything is allowed in quotes
                }
            }
            else if (isAlphaNumUS(c))
            {
                if (!isCaps(c))
                {
                    st.push({c, nullptr});
                }
                else
                {
                    int ind = c - 'A';
                    const tokenTrie *next = nullptr;
                    if (st.empty())
                        next = root->getChild(ind);
                    else if (st.top().second != nullptr)
                        next = st.top().second->getChild(ind);
                    st.push({c, next});
                }
            }
            else if (c == '"')
            {
                if (!st.empty())
                {
                    Message = "Unterminated sequence before opening quotes.";
                    error = true;
                    break;
                }
                isInQuotes = true;
            }
            else if (c == ' ')
            {
                push();
            }
            else if (c == ',')
            {
                push();
                tokens.push_back({tokenType::COMMA, ","});
            }
            else if (c == '(')
            {
                bracketCount++;
                if (!st.empty())
                {
                    Message = "Unterminated sequence before Opening Bracket.";
                    error = true;
                    break;
                }
                tokens.push_back({tokenType::BRACKET_OPEN, "("});
            }
            else if (c == ')')
            {
                if (bracketCount == 0)
                {
                    Message = "Invalid Closing Brackets.";
                    error = true;
                    break;
                }
                bracketCount--;
                push();
                tokens.push_back({tokenType::BRACKET_CLOSE, ")"});
            }
            else if (isOperator(c))
            {
                push();
                if (!tokens.empty())
                {
                    Token tp = tokens.back();
                    if (tp.type == OPERATOR)
                    {
                        std::string op = std::get<0>(tp.value);
                        op.push_back(c);
                        bool isValid = false;
                        for (auto &OP : OPERATORS)
                        {
                            if (OP == op)
                            {
                                isValid = true;
                                break;
                            }
                        }
                        if (!isValid)
                        {
                            Message = "Invalid operator " + op;
                            error = true;
                            break;
                        }
                        tokens.pop_back();
                        tokens.push_back({tokenType::OPERATOR, op});
                        continue;
                    }
                    tokens.push_back({tokenType::OPERATOR, std::string(1, c)});
                }
                else
                {
                    tokens.push_back({tokenType::OPERATOR, std::string(1, c)});
                }
            }
            else if (c == ';')
            {
                push();
                if (bracketCount > 0)
                {
                    Message = "Open brackets not closed.";
                    error = true;
                    break;
                }
                if (isInQuotes)
                {
                    Message = "Unclosed quotes.";
                    error = true;
                    break;
                }
            }
            else
            {
                Message = "Invalid character.";
                error = true;
                break;
            }
        }
        if (isInQuotes)
        {
            Message = "Unclosed quotes.";
            error = true;
        }
        return !error;
    }
};

#endif