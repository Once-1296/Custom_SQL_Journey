#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "../types/Token.hpp"
#include "../types/token_trie.hpp"

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
        char ops[] = {'+', '-', '*', '/', '>', '<', '=', '!', '&', '|', '~', '^'};
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

    void printTokens(const std::vector<Token> &tokens)
    {
        for (const auto &tok : tokens)
        {
            if (tok.value.index() == 0)
                std::cout << std::get<0>(tok.value);
            else
                std::cout << std::get<1>(tok.value);

            std::cout << " : " << printType(tok.type) << '\n';
        }
    }

    bool lex(std::string query, std::vector<Token> &tokens, std::string &Message)
    {
        if (!tokens.empty())
        {
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

        // Returns false if an overflow/underflow occurs
        auto push = [&tokens, &st, &Message](bool forceString = false) -> bool
        {
            if (st.empty())
                return true;

            tokenType type = tokenType::INT;

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
                st.pop();
            }
            std::reverse(true_s.begin(), true_s.end());

            if (forceString)
            {
                tokens.push_back({tokenType::FORCE_STR, true_s});
                return true;
            }

            // Check if string is a valid integer (including negative signs)
            bool isint = !true_s.empty();
            size_t start_idx = 0;
            if (true_s[0] == '-')
            {
                if (true_s.size() == 1)
                    isint = false;
                else
                    start_idx = 1;
            }

            for (size_t i = start_idx; i < true_s.size(); ++i)
            {
                if (true_s[i] < '0' || true_s[i] > '9')
                {
                    isint = false;
                    break;
                }
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
            {
                try
                {
                    int val = std::stoi(true_s);
                    tokens.push_back({type, val});
                }
                catch (const std::out_of_range &e)
                {
                    Message = "Integer overflow/underflow detected: " + true_s;
                    return false;
                }
                catch (const std::invalid_argument &e)
                {
                    tokens.push_back({tokenType::STR, true_s});
                }
            }
            else
            {
                tokens.push_back({type, true_s});
            }
            return true;
        };

        for (size_t i = 0; i < query.size(); ++i)
        {
            char c = query[i];

            if (isInQuotes)
            {
                if (c == '"')
                {
                    if (!push(true))
                    {
                        error = true;
                        break;
                    }
                    isInQuotes = false;
                }
                else
                {
                    st.push({c, nullptr});
                }
            }
            // Check for negative numbers (glued '-' before a digit when st is empty)
            else if (c == '-' && st.empty() && (i + 1 < query.size()) && isNum(query[i + 1]))
            {
                st.push({c, nullptr});
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
                if (!push())
                {
                    error = true;
                    break;
                }
            }
            else if (c == ',')
            {
                if (!push())
                {
                    error = true;
                    break;
                }
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
                if (!push())
                {
                    error = true;
                    break;
                }
                tokens.push_back({tokenType::BRACKET_CLOSE, ")"});
            }
            else if (isOperator(c))
            {
                if (!push())
                {
                    error = true;
                    break;
                }
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
                            // Message = "Invalid operator " + op;
                            // error = true;
                            // break;
                            // assume as two separate operators
                            tokens.push_back({tokenType::OPERATOR, std::string(1, c)});
                            continue;
                        }
                        tokens.pop_back();
                        tokens.push_back({tokenType::OPERATOR, op});
                        continue;
                    }
                }
                tokens.push_back({tokenType::OPERATOR, std::string(1, c)});
            }
            else if (c == ';')
            {
                if (!push())
                {
                    error = true;
                    break;
                }
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

        if (isInQuotes && !error)
        {
            Message = "Unclosed quotes.";
            error = true;
        }

        return !error;
    }
};

#endif