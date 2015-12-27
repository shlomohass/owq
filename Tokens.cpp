/**
 * Tokens.cpp
 * Author: shlomo hassid
 * 
 */

#include "Tokens.h"
#include "Lang.h"
#include <sstream> 

Tokens::Tokens() {
    tokens.reserve(30);
    type.reserve(30);
    priorty.reserve(30);
    flags.reserve(30);
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
void Tokens::addToken(string token, int priortyCode, TokenType tokenType) {
    addToken(token, priortyCode, tokenType, false);
}
void Tokens::addToken(string token, int priortyCode, TokenType tokenType, bool useFlags) {
    tokens.push_back(token);
    priorty.push_back(priortyCode);
    type.push_back(tokenType);
    if (useFlags && tokenType == TokenType::DELIMITER) {
        if (Lang::LangIsComparison(token)) {
            flags.push_back(TokenFlag::COMPARISON);
            comparisonFlag = true;
        } else if (Lang::LangIsOfCondition(token)) {
            flags.push_back(TokenFlag::CONDITION);
            conditionFlag = true;
        } else {
            flags.push_back(TokenFlag::NORMAL);
        }
    } else {
        flags.push_back(TokenFlag::NORMAL);
    }
    if(getSize() > 1) {
        int size = getSize();
        //if previous token before this one is a variable
        if (isOpenParenthesis(size-1) && isVar(size-2)) {
            priorty[size-2] = 2;
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
    if (type[index] == TokenType::NUMBER) {
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
    if(type[index] == TokenType::DELIMITER) {
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
    if (type[index] == TokenType::VAR) {
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
    if(tokens[index] == Lang::LangFindDelimiter("braketClose")) {
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
    if(tokens[index] == Lang::LangFindDelimiter("braketOpen")){
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
Tokens Tokens::extractContentOfParenthesis(int startParenthesisIndex, int endParenthesisIndex, int& extractionCount) {
    Tokens newToken;
    if(startParenthesisIndex < 0 || startParenthesisIndex > getSize() - 1) {
        stdError("token extraction, startIndex out of bounds");
        return newToken;
    }
    if(endParenthesisIndex < startParenthesisIndex || endParenthesisIndex > getSize()-1) {
        stdError("token extraction, endIndex out of bounds");
        return newToken;
    }
    //copy from current token-set to new token-set
    int i;
    int count = 0;	//the total count of values to extract
    for (i=startParenthesisIndex+1; i < endParenthesisIndex; i++) {
        newToken.addToken(tokens[i], priorty[i], type[i]);
        count++;
    }
    //erase the total number of tokens extracted including what is before and after
    for (i=0; i<(count+2); i++) {
        //the contents extracted
        tokens.erase    (tokens.begin()	 + startParenthesisIndex);
        type.erase      (type.begin()	 + startParenthesisIndex);
        priorty.erase   (priorty.begin() + startParenthesisIndex);
    }
    extractionCount = count + 2;
    tokens.insert(tokens.begin()+startParenthesisIndex, "RST");
    type.insert(type.begin()+startParenthesisIndex, TokenType::NONE);
    priorty.insert(priorty.begin()+startParenthesisIndex, 0);
    return newToken;
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
Tokens Tokens::extractInclusive(int startIndex, int endIndex, int& extractionCount) {
    Tokens newToken;
    if(startIndex < 0 || startIndex > getSize()-1){
        stdError("token extraction, startIndex out of bounds");
        return newToken;
    }
    if(endIndex < startIndex || endIndex > getSize()-1){
        stdError("token extraction, endIndex out of bounds");
        return newToken;
    }
    //copy from current token-set to new token-set
    int i;
    int count=0;	//the total count of values to extract
    for(i=startIndex; i < endIndex+1; i++){
        newToken.addToken(tokens[i], priorty[i], type[i]);
        count++;
    }
    //erase
    for(i=0; i<(count); i++){	//erase the total number of tokens extracted including what is before and after
        //the contents extracted
        tokens.erase(tokens.begin() + startIndex);
        type.erase(type.begin() + startIndex);
        priorty.erase (priorty.begin() + startIndex);
    }
    extractionCount = count;
    tokens.insert(tokens.begin()+startIndex, "RST");
    type.insert(type.begin()+startIndex, TokenType::NONE);
    priorty.insert(priorty.begin()+startIndex,0);
    return newToken;
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
    cout << "    TOKENS --> { ";
    for(int i=0; i<getSize(); i++){
        cout << "'" << tokens[i] << "' ";
    }
    cout << "}" << endl;
}

void Tokens::renderTokensJoined() {
    cout << "    TOKENS-JOINED --> ";
    for(int i=0; i<getSize(); i++) {
        cout << tokens[i] << " ";
    }
    cout << endl << endl;
    cout << "-------------------------------------------------------------------" << endl << endl;
}
void Tokens::renderTokenType(){
    string str;
    cout << "{ ";
    for(int i=0; i<getSize(); i++){
        switch(type[i]) {
            case TokenType::DELIMITER:  str = "DLM"; break;
            case TokenType::NUMBER:	str = "STR"; break;
            case TokenType::STRING:	str = "NUM"; break;
            case TokenType::VAR:	str = "VAR"; break;
            case TokenType::KEYWORD:	str = "KEY"; break;
            case TokenType::NONE:	str = "NON"; break;
        }
        cout << "'" << str << "' ";
    }
    cout << "}" << endl;
}

void Tokens::renderTokenPriorty(){
    cout << "{ ";
    for(int i=0; i<getSize(); i++){
        cout << "[" << priorty[i] << "] ";
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
    return priorty[index];
}

/**
 * Search for the highest priority value and return its index
 * @param priortyCode
 * @return
 */
int Tokens::getHighestOperatorPriorityIndex(int& priortyCode) {
    int index = 0;
    int highest = -10;
    for (int i = 0; i < getSize(); i++){
        if (priorty[i] > highest) {
            highest = priorty[i];
            priortyCode = highest;
            index = i;
        }
    }
    return index;
}
/**
 * Get a specific token at index
 * @param int index
 * @return string
 */
string Tokens::getToken(int index) {
    if (index < 0 || index >= getSize()) {
        cout << index << "/" << getSize()-1 << endl;
        stringstream s;
        s << index;
        stringstream ss;
        ss << getSize();
        stdError("get token, index(" + s.str() + ") is out of range, token size currently " + ss.str());
        return ".none.";
    }
    return tokens[index];
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
    return flags.at(index);
}
/** Find closing Parenthesis token of open index
 * 
 * @param integer openIndex
 * @return  integer
 */
int Tokens::getMatchingCloseParenthesis(int openIndex) {
    int i;
    int nested = 0;
    string bracketOpen = Lang::LangFindDelimiter("braketOpen");
    string bracketClose = Lang::LangFindDelimiter("braketClose");
    for (i = openIndex + 1; i < getSize(); i++ ) {
        if (tokens[i] == bracketOpen) {
            nested++;
        } else if (tokens[i] == bracketClose && nested > 0) {
            nested--;
        } else if ( tokens[i] == bracketClose && nested == 0 ) {
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
        stdError("pop index out of range");
        return;
    }
    tokens.erase(tokens.begin() + index);
    type.erase(type.begin() + index);
    priorty.erase(priorty.begin() + index);
    flags.erase(flags.begin() + index);
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
bool Tokens::pushBefore(int index, string token, int pri, TokenType type) {
    auto pos_tokens = this->tokens.begin();
    auto pos_priorty = this->priorty.begin();
    auto pos_type = this->type.begin();
    auto pos_flags = this->flags.begin();
    if (index >= this->tokens.size() || index < 0) { return false; }
    this->tokens.insert(pos_tokens + index, token);
    this->priorty.insert(pos_priorty + index, pri);
    this->type.insert(pos_type + index, type);
    this->flags.insert(pos_flags + index, TokenFlag::NORMAL);
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
bool Tokens::pushAfter(int index, string token, int pri, TokenType type) {
    if (index + 1 >= this->tokens.size()) {
        this->tokens.push_back(token);
        this->priorty.push_back(pri);
        this->type.push_back(type);
        this->flags.push_back(TokenFlag::NORMAL);
    } else {
        auto pos_tokens = this->tokens.begin();
        auto pos_priorty = this->priorty.begin();
        auto pos_type = this->type.begin();
        auto pos_flags = this->flags.begin();
        advance(pos_tokens, index + 1);
        advance(pos_priorty, index + 1);
        advance(pos_type, index + 1);
        advance(pos_flags, index + 1);
        this->tokens.insert(pos_tokens, token);
        this->priorty.insert(pos_priorty, pri);
        this->type.insert(pos_type, type);
        this->flags.insert(pos_flags, TokenFlag::NORMAL);
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
    if (type[index] == TokenType::KEYWORD) {
        return true;
    } else {
        return false;
    }
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
    if (type[index] == TokenType::STRING) {
        return true;
    } else {
        return false;
    }
}
/** clear vectors
 * 
 */
void Tokens::clear() {
    type.clear();
    priorty.clear();
    tokens.clear();
    flags.clear();
    comparisonFlag = false;
    conditionFlag = false;
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
