/* 
 * File:   Tokens.h
 * Author: shlomo hassid
 *
 */

#ifndef TOKENS_H
#define	TOKENS_H

#include <iostream>
#include <vector>
#include "Token.h"

using namespace std;

/**
 * Functions to hold script terms that characterize a line
 */
class Script;
class Tokens {
    
    friend class Parser;
    
private:
    vector<Token> tokens;
    //Flags which indicates if the set of tokens has special token which needs grouping
    bool comparisonFlag;
    bool conditionFlag;
    
public:
    Tokens();
    void addToken(string _token, int priortyCode, TokenType tokenType, bool useFlags);
    void addToken(string _token, int priortyCode, TokenType tokenType);
    void addToken(Token _token);
    bool isNumber(int index);
    bool isString(int index);
    bool isDelimiter(int index);
    bool isVar(int index);
    bool isCloseParenthesis(int index);
    bool isOpenParenthesis(int index);
    bool isKeyWord(int index);
    int  getTokenPriorty(int index);
    void clear();
    Tokens extractContentOfParenthesis(int startIndex, int endIndex, int& extractionCount, Script* script);
    Tokens extractContentOfParenthesis(int startIndex, int endIndex, int& extractionCount, Script* script, bool setPointer);
    Tokens extractInclusive(int startIndex, int endIndex,int& extractionCount, Script* script);
    Tokens extractInclusive(int startIndex, int endIndex,int& extractionCount, Script* script, bool setPointer);
    int getHighestOperatorPriorityIndex(int& priortyCod);
    int getMatchingCloseParenthesis(int openIndex);
    string getToken(int index);
    Token* getTokenObject(int index);
    bool setHasComparison();
    bool setHasCondition();
    TokenFlag getTokenFlag(int index);
    void renderTokens();
    void renderTokensJoined();
    void renderTokenType();
    void renderTokenPriorty();
    void pop(int index);
    bool pushBefore(int index, string _token, int pri, TokenType type);
    bool pushAfter(int index, string _token, int pri, TokenType type);
    static void stdError(string msg);
    int getSize();
    virtual ~Tokens();
};

#endif	/* TOKENS_H */

