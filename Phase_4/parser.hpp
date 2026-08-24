#ifndef PARSER_HPP
#define PARSER_HPP

#include "parse_tree.hpp"
#include "custom_catalog.hpp"
#include <stack>

class Parser
{
private:
    catalog *cata;
    std::vector<std::string> KEYWORDS = {
        "SHOW", "CREATE", "LINK", "UNLINK", "DELETE",
        "SCHEMA", "PRIMARY", "KEY",
        "TABLE", "TABLES", "DATABASE", "DATABASES",
        "INSERT", "INTO", "UPDATE", "SET",
        "SELECT", "FROM", "WHERE"};

public:
    Parser() : cata(nullptr)
    {
    }
    std::string removeTrails(std::string &in)
    {
        std::string out = "";
        bool started = false, addSpace = false;
        for (auto &c : in)
        {
            if (c == ' ')
            {
                addSpace = (started && true);
            }
            else
            {
                started = true;
                if (addSpace)
                    out.push_back(' ');
                addSpace = false;
                out.push_back(c);
            }
        }
        return out;
    }

    bool isAlphaNum(char &c)
    {
        return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
    }
    bool isOperator(char &c)
    {
        return ((c == '>') || (c == '=') || (c == '<'));
    }
    bool parseIntoTokens(std::string query, Token &root)
    {
        while (!query.empty() && query.back() == ' ')
            query.pop_back();

        try
        {
            if (query.empty() || query[0] == ';' || query.back() != ';')
            {
                throw std::invalid_argument("Query is blank/ does not end with ;");
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << e.what() << '\n';
            return false;
        }

        std::stack<Token *> stk;
        stk.push(&root);
        std::string current = "";
        bool nextShouldBeSpace = false;
        bool error = false;
        std::string errorMessage = "";
        int mode = 0;
        // 0 -> normal
        // 1 -> inside bracket
        // 2 -> inside string
        // 3 -> inside string inside bracket

        auto push_current = [&stk, &current, this](bool trailify = false) -> void
        {
            current = this->removeTrails(current);
            if (!current.empty())
            {
                TokenType type = TokenType::STR;
                Token *tok = new Token(type);
                tok->setValue(Value(current));
                if (!stk.empty())
                {
                    stk.top()->addChild(tok);
                }
                current.clear();
            }
        };

        for (auto &c : query)
        {
            if (c != ' ' && nextShouldBeSpace)
            {
                error = true;
                errorMessage = "Missing a Space Somewhere.";
                break;
            }
            nextShouldBeSpace = false;

            if (c == ' ')
            {
                if (mode == 2 || mode == 3)
                {
                    current.push_back(c);
                }
                else
                {
                    push_current();
                }
            }
            else if (isAlphaNum(c))
            {
                current.push_back(c);
            }
            else if (c == ',')
            {
                if (mode == 2 || mode == 3)
                {
                    error = true;
                    errorMessage = "Cannot put comma in quotes.";
                    break;
                }
                push_current();
                Token *sep = new Token(TokenType::COMMA);
                if (!stk.empty())
                    stk.top()->addChild(sep);
            }
            else if (c == '*')
            {
                if (mode != 0 || !current.empty())
                {
                    error = true;
                    errorMessage = "Cannot put * in quotes/brackets or lack of proper space.";
                    break;
                }
                nextShouldBeSpace = true;
                Token *sep = new Token(TokenType::STAR);
                if (!stk.empty())
                    stk.top()->addChild(sep);
            }
            else if (c == '(')
            {
                if (mode == 2 || mode == 3)
                {
                    error = true;
                    errorMessage = "Cannot put bracket in quotes.";
                    break;
                }
                push_current();
                TokenType type = TokenType::BRACKET_VALUES;
                Token *tok = new Token(type);
                if (!stk.empty())
                {
                    stk.top()->addChild(tok);
                    stk.push(stk.top()->getChildren().back());
                }
                mode = 1;
            }
            else if (c == ')')
            {
                if (mode != 1)
                {
                    error = true;
                    errorMessage = "Closing bracket in wrong place";
                    break;
                }
                push_current();
                if (stk.empty() || stk.top()->getType() != TokenType::BRACKET_VALUES)
                {
                    error = true;
                    errorMessage = "Closing bracket in wrong place";
                    break;
                }
                stk.pop();
                mode = (!stk.empty() && stk.top()->getType() == TokenType::BRACKET_VALUES) ? 1 : 0;
            }
            else if (c == '"')
            {
                if (mode == 0 || mode == 1)
                {
                    if (!current.empty())
                    {
                        error = true;
                        errorMessage = "Quoted content needs space before.";
                        break;
                    }
                    TokenType type = TokenType::QUOTES;
                    Token *tok = new Token(type);
                    if (!stk.empty())
                    {
                        stk.top()->addChild(tok);
                        stk.push(stk.top()->getChildren().back());
                    }
                    mode = (mode == 1) ? 3 : 2;
                }
                else if (mode == 2 || mode == 3)
                {
                    push_current(true);
                    if (stk.empty() || stk.top()->getType() != TokenType::QUOTES)
                    {
                        error = true;
                        errorMessage = "Closing quote is in wrong place";
                        break;
                    }
                    stk.pop();
                    nextShouldBeSpace = true;
                    mode = (mode == 3) ? 1 : 0;
                }
                else
                {
                    error = true;
                    errorMessage = "Invalid mode.";
                    break;
                }
            }
            else if (isOperator(c))
            {
                if (mode == 2 || mode == 3)
                {
                    error = true;
                    errorMessage = "Operator not allowed in quotes.";
                    break;
                }
                push_current();
                current.push_back(c);
                push_current();
            }
            else if (c == ';')
            {
                if (mode != 0)
                {
                    error = true;
                    errorMessage = "Missing closing bracket/quote.";
                    break;
                }
                push_current();
            }
        }

        if (error)
        {
            std::cout << errorMessage << std::endl;
        }

        return !error;
    }
    void printParsedTokens(Token &root)
    {
        std::string indent(4, ' ');
        std::string currentIndent = "";
        // std::cout<<"hello\n";
        auto dfs = [&indent, &currentIndent](auto &self, Token &cur) -> void
        {
            std::string start = currentIndent;
            bool has_child = false;
            if (cur.getType() == TokenType::BRACKET_VALUES || cur.getType() == TokenType::QUOTES)
            {
                // std::cout<<"2\n";
                std::cout<<start<<((cur.getType() == TokenType::BRACKET_VALUES)?"(":"\"")<<std::endl;
                currentIndent += indent;
                has_child = true;
            }
            else if (cur.getType() != ROOT)
            {
                // std::cout<<"1\n";
                start += cur.getValue().AsVarchar();
                std::cout << start << '\n';
            }
            else
            {
                // std::cout<<"3\n";
                has_child = true;
            }
            if (has_child)
            {
                for (auto &child : cur.getChildren())
                {
                    self(self, *child);
                }
            }
            if (cur.getType() == TokenType::BRACKET_VALUES || cur.getType() == TokenType::QUOTES)
            {
                for (uint32_t i = 0; i < indent.size(); i++)
                    currentIndent.pop_back();
                std::cout<<currentIndent<<((cur.getType() == TokenType::BRACKET_VALUES)?")":"\"")<<std::endl;
            }
        };
        dfs(dfs, root);
    }
};

#endif