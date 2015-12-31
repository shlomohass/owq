/**
 * Tokens.cpp
 * Author: shlomo hassid
 * 
 */

#include "Tokens.h"
#include "Lang.h"
#include "Script.h"
#include <sstream> 

Tokens::Tokens() {
    tokens.reserve(30);
    comparisonFlag = false;
    conditionFlag  =  false;
}

/**
 * Principle agent
 * Serves to create the token set, add as many as needed
 * @param token
 * @param priortyCode
 * @param tokenType
 */
void Tokens::addToken(Token _token) {
    tokens.push_back(_token);
}
void Tokens::addToken(string _token, int priortyCode, TokenType tokenType) {
    addToken(_token, priortyCode, tokenType, false);
}
void Tokens::addToken(string _token, int priortyCode, TokenType tokenType, bool useFlags) {
    Token token(_token, priortyCode, tokenType, TokenFlag::NORMAL); 
    if (useFlags && tokenType == TokenType::DELIMITER) {
        if (Lang::LangIsComparison(_token)) {
            token.setFlag(TokenFlag::COMPARISON);
            comparisonFlag = true;
        } else if (Lang::LangIsOfCondition(_token)) {
            token.setFlag(TokenFlag::CONDITION);
            conditionFlag = true;
        }
    }
    tokens.push_back(token);
    if(getSize() > 1) {
        int size = getSize();
        //if previous token before this one is a variable
        if (isOpenParenthesis(size-1) && isVar(size-2)) {
            tokens[size-2].priority = 2;
        }
    }
}

/**
 * Returns true or false indicating if the index is a number
 * @param index of token, bound checking is implemented
 * @return true or false
 */
bool Tokens::isNumber(int index) {
    if (index < 0 || index > getSize() - 1){
        return false;
    }
    if (tokens[index].type == TokenType::NUMBER) {
        return true;
    }
    return false;
}

/**
 * Returns true or false indicating if the index is a delimiter
 * @param index of token, bound checking is implemented
 * @return true or false
 */
bool Tokens::isDelimiter(int index) {
    if(index < 0 || index > getSize()-1){
        return false;
    }
    if(tokens[index].type == TokenType::DELIMITER) {
        return true;
    }
    return false;
}

/**
 * Returns true or false indicating if the index is a number
 * @param index of token, bound checking is implemented
 * @return true or false
 */
bool Tokens::isVar(int index) {
    if (index < 0 || index > getSize()-1) {
        return false;
    }
    if (tokens[index].type == TokenType::VAR) {
        return true;
    }
    return false;
}

/**
 * Indicate whether or not the index supplied is that of a closed parenthesis
 * @param index
 * @return
 */
bool Tokens::isCloseParenthesis(int index) {
    bool isDelim = isDelimiter(index);
    if(!isDelim) return false;
    if(tokens[index].token == Lang::LangFindDelimiter("braketClose")) {
        return true;
    }
    return false;
}
/**
 * Indicate whether or not the index supplied is that of an open parenthesis
 * @param index
 * @return
 */
bool Tokens::isOpenParenthesis(int index) {
    bool isDelim = isDelimiter(index);
    if(!isDelim) return false;
    if(tokens[index].token == Lang::LangFindDelimiter("braketOpen")){
        return true;
    }
    return false;
}

/**
 *
 * Retract the contents between the two indexes and return everything excluding the values at the indexes
 *
 * example:
 * 		token set of:
 * 		'(' 'varA' + '3244' ')'
 * 		will return tokens
 * 		'VarA' + '3244'
 *
 * After extracting the contents, RST is used to replace the content that was just extracted
 * @param startParenthesisIndex	index of the open parenthesis '('
 * @param endParenthesisIndex	index of the close parenthesis ')'
 * @param extractionCount		place the store the total number of items extracted from this token set
 * @return
 */
Tokens Tokens::extractContentOfParenthesis(int startParenthesisIndex, int endParenthesisIndex, int& extractionCount, Script* script) {
    return extractContentOfParenthesis(startParenthesisIndex, endParenthesisIndex, extractionCount, script, false);
}
Tokens Tokens::extractContentOfParenthesis(int startParenthesisIndex, int endParenthesisIndex, int& extractionCount, Script* script, bool setPointer) {
    Tokens newTokenSet;
    if(startParenthesisIndex < 0 || startParenthesisIndex > getSize() - 1) {
        stdError("token extraction, startIndex out of bounds");
        return newTokenSet;
    }
    if(endParenthesisIndex < startParenthesisIndex || endParenthesisIndex > getSize()-1) {
        stdError("token extraction, endIndex out of bounds");
        return newTokenSet;
    }
    //copy from current token-set to new token-set
    int i;
    int count = 0;	//the total count of values to extract
    for (i = startParenthesisIndex + 1; i < endParenthesisIndex; i++) {
        newTokenSet.addToken(tokens[i]);
        count++;
    }
    //erase the total number of tokens extracted including what is before and after
    for (i=0; i<(count+2); i++) {
        //the contents extracted
        tokens.erase(tokens.begin() + startParenthesisIndex);
    }
    extractionCount = count + 2;
    if (setPointer && script->code.size() > 0) {
        script->internalStaticPointer += 1;
        Token rstToken("RST",0,TokenType::RST,TokenFlag::NORMAL, script->internalStaticPointer);
        tokens.insert(tokens.begin()+startParenthesisIndex, rstToken);
        //Mark the last operation to set result with pointer:
        script->code.back().setPointer(script->internalStaticPointer);
        
    } else {
        Token rstToken("RST",0,TokenType::RST,TokenFlag::NORMAL, 0);
        tokens.insert(tokens.begin() + startParenthesisIndex, rstToken);
    }
    return newTokenSet;
}

/**
 * Extract , inclusively, everything between startIndex and endIndex
 *
 * After extracting the contents, RST is used to replace the content that was just extracted
 *
 * @param startIndex
 * @param endIndex
 * @param extractionCount
 * @return
 */
Tokens Tokens::extractInclusive(int startIndex, int endIndex, int& extractionCount, Script* script) {
    return extractInclusive(startIndex, endIndex, extractionCount, script, false);
}
Tokens Tokens::extractInclusive(int startIndex, int endIndex, int& extractionCount, Script* script, bool setPointer) {
    Tokens newTokenSet;
    if(startIndex < 0 || startIndex > getSize()-1){
        stdError("token extraction, startIndex out of bounds");
        return newTokenSet;
    }
    if(endIndex < startIndex || endIndex > getSize()-1){
        stdError("token extraction, endIndex out of bounds");
        return newTokenSet;
    }
    //copy from current token-set to new token-set
    int i;
    int count=0; //the total count of values to extract
    for(i=startIndex; i < endIndex+1; i++){
        newTokenSet.addToken(tokens[i]);
        count++;
    }
    //erase
    for (i=0; i<(count); i++){ //erase the total number of tokens extracted including what is before and after
        //the contents extracted
        tokens.erase(tokens.begin() + startIndex);
    }
    //Set a RST:
    extractionCount = count;
    if (setPointer && script->code.size() > 0) {
        script->internalStaticPointer += 1;
        Token rstToken("RST",0,TokenType::RST, TokenFlag::NORMAL, script->internalStaticPointer);
        tokens.insert(tokens.begin()+startIndex, rstToken);
        //Mark the last operation to set result with pointer:
        script->code.back().setPointer(script->internalStaticPointer);
    } else {
        Token rstToken("RST",0,TokenType::RST,TokenFlag::NORMAL, 0);
        tokens.insert(tokens.begin() + startIndex, rstToken);
    }
           
    return newTokenSet;
}

/** Report error to user
 * 
 * @param msg
 */
void Tokens::stdError(string msg) {
    cout << "error: " << msg << endl;
}

/** Expose token:
 * 
 */
void Tokens::renderTokens() {
    cout << "   TOKENS        --> { ";
    for(int i=0; i<getSize(); i++){
        cout << "'" << tokens[i].token << "' ";
    }
    cout << "}" << endl;
}

void Tokens::renderTokensJoined() {
    cout << "   TOKENS-JOINED   --> ";
    for(int i=0; i<getSize(); i++) {
        cout << tokens[i].token << " ";
    }
    cout << endl << endl;
}
void Tokens::renderTokenType() {
    string str;
    cout << "   TOKENS-TYPE   --> { ";
    for(int i=0; i<getSize(); i++){
        switch(tokens[i].type) {
            case TokenType::DELIMITER:  str = "DLM"; break;
            case TokenType::NUMBER:	str = "NUM"; break;
            case TokenType::STRING:	str = "STR"; break;
            case TokenType::VAR:	str = "VAR"; break;
            case TokenType::KEYWORD:	str = "KEY"; break;
            case TokenType::NONE:	str = "NON"; break;
            case TokenType::RST:        str = "RST"; break;
            default:
                str = "UNKNOWN";
        }
        if (str == "RST") {
            cout << "'" << str << ":" << tokens[i].rstPos << "' ";
        } else {
            cout << "'" << str << "' ";
        }
    }
    cout << "}" << endl;
}

void Tokens::renderTokenPriorty(){
    cout << "   TOKENS-PRIORITY --> ";
    for(int i=0; i<getSize(); i++){
        cout << "[" << tokens[i].priority << "] ";
    }
    cout << "}" << endl;
}

/**
 * Return the priority / weight associated with token at an index
 * @param integer index
 * @return integer
 */
int Tokens::getTokenPriorty(int index) {
    if ( index < 0 || index > getSize() - 1 ){
        stdError("getToken priority out of bounds");
        return 0;
    }
    return tokens[index].priority;
}

/**
 * Search for the highest priority value and return its index
 * @param priortyCode
 * @return
 */
int Tokens::getHighestOperatorPriorityIndex(int& priortyCode) {
    int index = 0;
    int highest = -10;
    int size = getSize();
    for (int i = 0; i < size; i++){
        if (tokens[i].priority > highest) {
            highest = tokens[i].priority;
            priortyCode = highest;
            index = i;
        }
    }
    return index;
}
/** Get a specific token at index
 * 
 * @param int index
 * @return string
 */
string Tokens::getToken(int index) {
    if (index < 0 || index >= getSize()) {
        return ".none.";
    }
    return tokens[index].token;
}
/** Get a specific token object at index
 * 
 * @param int index
 * @return Token
 */
Token* Tokens::getTokenObject(int index) {
    if (index < 0 || index >= getSize()) {
        return nullptr;
    }
    return &tokens[index];
}
/** Check if a grouping flag is true or not
 * 
 * @param openIndex
 * @return 
 */
bool Tokens::setHasComparison() {
    return comparisonFlag;
}
bool Tokens::setHasCondition() {
    return conditionFlag;
}
TokenFlag Tokens::getTokenFlag(int index) {
    if (index < 0 || index >= getSize()) {
        return TokenFlag::UNFLAG;
    }
    return tokens[index].flag;
}
/** Find closing Parenthesis token of open index
 * 
 * @param integer openIndex
 * @return  integer
 */
int Tokens::getMatchingCloseParenthesis(int openIndex) {
    int i;
    int nested = 0;
    int size = getSize();
    string bracketOpen = Lang::LangFindDelimiter("braketOpen");
    string bracketClose = Lang::LangFindDelimiter("braketClose");
    for (i = openIndex + 1; i < size; i++ ) {
        if (tokens[i].token == bracketOpen) {
            nested++;
        } else if (tokens[i].token == bracketClose && nested > 0) {
            nested--;
        } else if (tokens[i].token == bracketClose && nested == 0 ) {
            return i;
            break;
        }
    }
    return openIndex;
}
/** Remove token at index
 * 
 * @param integer index
 */
void Tokens::pop(int index) {
    if (index < 0 || index > getSize()-1) {
        stdError("pop token index out of range");
        return;
    }
    tokens.erase(tokens.begin() + index);
}

/** Push a token before a index:
 * 
 * @param string token
 * @param int pri
 * @param TokenType type
 * @param int index
 * @return boolean
 * 
 */
bool Tokens::pushBefore(int index, string _token, int pri, TokenType type) {
    auto pos_tokens = tokens.begin();
    if (index >= (int)tokens.size() || index < 0) { return false; }
    Token token(_token, pri, type, TokenFlag::NORMAL);
    tokens.insert(pos_tokens + index, token);
    return true;
}
/** Push a token after a index:
 * 
 * @param int index
 * @param string token
 * @param int pri
 * @param TokenType type
 * @return boolean
 * 
 */
bool Tokens::pushAfter(int index, string _token, int pri, TokenType type) {
    Token token(_token, pri, type, TokenFlag::NORMAL);
    if (index + 1 >= (int)this->tokens.size()) {
        this->tokens.push_back(token);
    } else {
        auto pos_tokens = this->tokens.begin();
        advance(pos_tokens, index + 1);
        this->tokens.insert(pos_tokens, token);
    }
    return true;
}
    
/** Evaluate a token and check if its a keyword
 * 
 * @param index
 * @return boolean
 */
bool Tokens::isKeyWord(int index) {
    if (index < 0 || index >= getSize()) {
        stdError("index for keyword check is out of bounds");
        return false;
    }
    if (tokens[index].type == TokenType::KEYWORD) {
        return true;
    }
    return false;
}
/** Evaluate a token and check if its a string
 * 
 * @param index
 * @return boolean
 */
bool Tokens::isString(int index) {
    if (index < 0 || index >= getSize()) {
        stdError("index for keyword check is out of bounds");
        return false;
    }
    if (tokens[index].type == TokenType::STRING) {
        return true;
    }
    return false;
}
/** clear tokens
 * 
 */
void Tokens::clear() {
    tokens.clear();
    comparisonFlag = false;
    conditionFlag  = false;
}
/** get tokens count 
 * 
 * @return int
 * 
 */
int Tokens::getSize() {
    return (int)tokens.size();
}

Tokens::~Tokens() {
    clear();
}
