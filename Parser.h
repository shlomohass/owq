/* 
 * File:   Parser.h
 * Author: shlomo hassid
 *
 */

#ifndef PARSER_H
#define	PARSER_H

using namespace std;

#include "Tokens.h"
#include "Lang.h"
#include "Script.h"


/**
 *
 * Parser functions as the workhorse of our script compilation
 *
 * It takes an expression and determines meaning
 */
class Parser {
    
    string expression;		//expression to evaluate and compile
    int expressionIndex;	//the index into the expression; i.e expression[expressionIndex]
    string currentToken;	//the currentToken as a function of expression

    vector<int> marks;		//helps to group related code branches: functions, if, while

    TokenType currentTokenType;	//describes the current token type

    //---------------------------------------------------------
    // vital workers
    //---------------------------------------------------------
    void 	tokenize(string& exp, Tokens& token);
    int   	compiler(Script* script, Tokens& token, int rCount);
    string 	getToken();
    void 	mark(int markType);
    int  	unmark();
    
    //----------------------------------------------------------
    // Aux- helper functions
    //----------------------------------------------------------
    bool whileNotDelimiter(int currentPos);
    bool isDelimiter(const string& c);
    bool isDelimiter(const char& c);
    bool isSpace(const char& c);
    bool isSpace(const string& c);
    bool isQstring(const char& c);
    bool isQstring(const string& c);
    bool isLetter(const char& c);
    bool isLetter(const string& c);
    bool isDigit(const char& c);
    bool isDigit(const string& c);
    int  getDelimiterPriorty();
    bool isKeyword(string s);
    bool hasCommas(Tokens& token);
    
public:
    Parser();
    
    //main work horses
    int compile(Script* script, string exp);
    int compile(Script* script, string exp, bool debug);
    
    virtual ~Parser();
    //General:
    string toLowerString(string *s);
    string toUpperString(string *s);
};

#endif	/* PARSER_H */

