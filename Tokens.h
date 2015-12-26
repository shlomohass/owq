/* 
 * File:   Tokens.h
 * Author: shlomo hassid
 *
 */

#ifndef TOKENS_H
#define	TOKENS_H

#include <iostream>
#include <vector>

using namespace std;

enum TokenType { NONE, NUMBER, STRING, DELIMITER, VAR, KEYWORD };
enum TokenFlag { NORMAL, COMPARISON, CONDITION };

/**
 * Functions to hold script terms that characterize a line
 */
class Tokens {
    
private:
    //! contains the dynamic array of individual string tokens
    vector<string> tokens;
    //! contains priority values for each token; purpose is to allow for faster compilation
    //! functions as a weighting system for each token
    vector<int>	priorty;
    //! describe what the token is: string/variable, number, delimiter
    vector<TokenType> type;
    // Hold tokens flags for grouping indicators
    vector<TokenFlag> flags;
    //Flags which indicates if the set of tokens has special token which needs grouping
    bool comparisonFlag;
    bool conditionFlag;
public:
    Tokens();
    void addToken(string token, int priortyCode, TokenType tokenType, bool useFlags);
    void addToken(string token, int priortyCode, TokenType tokenType);
    bool isNumber(int index);
    bool isString(int index);
    bool isDelimiter(int index);
    bool isVar(int index);
    bool isCloseParenthesis(int index);
    bool isOpenParenthesis(int index);
    bool isKeyWord(int index);
    int  getTokenPriorty(int index);
    void clear();
    Tokens extractContentOfParenthesis(int startIndex, int endIndex, int& extractionCount);
    Tokens extractInclusive(int startIndex, int endIndex,int& extractionCount);
    int getHighestOperatorPriorityIndex(int& priortyCod);
    int getMatchingCloseParenthesis(int openIndex);
    string getToken(int index);
    bool setHasComparison();
    bool setHasCondition();
    TokenFlag getTokenFlag(int index);
    void renderTokens();
    void renderTokensJoined();
    void renderTokenType();
    void renderTokenPriorty();
    void pop(int index);
    bool pushBefore(int index, string token, int pri, TokenType type);
    bool pushAfter(int index, string token, int pri, TokenType type);
    static void stdError(string msg);
    int getSize();
    virtual ~Tokens();
};

#endif	/* TOKENS_H */

