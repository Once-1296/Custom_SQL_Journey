#ifndef SELECT_CALL_HPP
#define SELECT_CALL_HPP

#include "../custom_catalog.hpp"
#include "command_validation_helpers.hpp"
#include "../types/Token.hpp"
#include "row_level_query_helpers.hpp"
#include <vector>


bool selectQry(std::vector<Token>&tokens, std::string &Message, catalog *&cata)
{
    if(cata == nullptr){
        Message = "No Database is linked";
        return false;
    }
    uint32_t bigNum = 100000000;
    std::map<std::string, uint32_t> kwordMap = {{"SELECT",bigNum},{"FROM",bigNum},{"WHERE",bigNum},{"ORDERBY",bigNum},{"LIMIT",bigNum}};
    for(uint32_t i = 0;i < tokens.size();i++){
        if(tokens[i].type == tokenType::KEYWORD){
            std::string kword = std::get<0>(tokens[i].value);
            if(kwordMap.contains(kword)){
                if(kwordMap[kword] != bigNum){
                    Message = kword + " is repeated.";
                    return false;
                }
                kwordMap[kword] = i;
            }
        }
    }
    if(kwordMap["SELECT"] != 0 || kwordMap["FROM"] == bigNum){
        Message = "Missing SELECT/FROM in query.";
        return false;
    }
    std::vector<std::string> kwords = {"SELECT","FROM","WHERE","ORDERBY","LIMIT"};
    std::vector<uint32_t> impInd(5);
    for(uint32_t i = 0; i< 5;i++){
        impInd[i] = kwordMap[kwords[i]];
    }
    auto fromRes = fromHelper(tokens, Message, cata, impInd[1]);
    if(!fromRes.first){
        Message += "\n : Error near expected FROM.";
        return false;
    }
    std::string tableName = std::get<0>(tokens[impInd[1]+1].value);
    std::vector<std::string> qryCols, outCols;
    bool selectRes = selectHelper(tokens, Message, cata, fromRes.second,impInd[1]-1,qryCols, outCols, 0);
    if(!selectRes){
        Message += "\n : error near SELECT";
        return false;
    }
    auto qryRes = cata->Query(tableName,qryCols,std::move(std::make_unique<ConstantValueExpression>(ConstantValueExpression(1))),outCols);
    if(!std::get<0>(qryRes)){
        Message = "Error in executing select query.";
        return false;
    }
    Schema outSchema = std::get<1>(qryRes);
    std::vector<Tuple> qryRows = std::get<2>(qryRes);
    if(impInd[2] != bigNum){
        // TODO: Where clause work
    }
    if(impInd[3] != bigNum){
        // TODO: Orderby clause work
    }
    if(impInd[4] != bigNum){
        // TODO: Limit clause work
    }
    print_table(outSchema,qryRows);
    std::cout<<"Fetched "<<qryRows.size()<<" rows successfully."<<std::endl;
    return true;
}

#endif 