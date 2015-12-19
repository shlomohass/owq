/* 
 * File:   Parser.cpp
 * Author: shlomo hassid
 *
 */
#include "Parser.h"
#include "Lang.h"

Parser::Parser() {

}
/**
 * Indicate whether or not a char constant c is one of our defined delimiters
 * Note: delimiters are used to separate individual tokens
 * @param c
 * @return return true if it is a delimiter otherwise false
 */
bool Parser::isDelimiter(const string& c) {
    return Lang::LangIsDelimiter(c);
}
bool Parser::isDelimiter(const char& c) {
    return Lang::LangIsDelimiter(c);
}
/** Check whether a char is space or not
 * 
 * @param char c
 * @return bool
 * 
 */
bool Parser::isSpace(const char& c) {
    return (c == Lang::LangFindDelimiter("space")) ? true : false;
}

/** Check whether we entering using a string:
 * @param char c
 * @return bool
 */
bool Parser::isQstring(const char& c) {
    return c == Lang::LangStringIndicator;
}
/** Validate naming chars:
 * 
 * @param char|string c
 * @return bool
 */
bool Parser::isLetter(const char& c) {
    return Lang::LangIsNamingAllowed(c);
}
bool Parser::isLetter(const string& c) {
    return Lang::LangIsNamingAllowed(c);
}

/**
 * Indicate true of false if a specific character constant, c, is a digit or not
 * @param c
 * @return
 */
bool Parser::isDigit(const char& c) {
	bool ret=false;
	switch(c){
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
            ret = true;
            break;
	default:
            ret = false;
            break;
	}
	return ret;
}

/**
 * Indicate if supplied string, s, is a keyword
 * @param s
 * @return
 */
bool Parser::isKeyword(string s) {
    if (Lang::LangIsKeyword(s)) {
        currentTokenType = TokenType::KEYWORD;
        return true;
    }
    return false;
}
/** Parse any string to lower ASCII chars
 * 
 * @param s
 * @return 
 */
string Parser::toLowerString(string *s) {
    string outBuffer = *s;
    transform(s->begin(), s->end(), outBuffer.begin(), ::tolower);
    return outBuffer;
}
/** Parse any string to lower ASCII chars
 * 
 * @param s
 * @return 
 */
string Parser::toUpperString(string *s) {
    string outBuffer = *s;
    transform(s->begin(), s->end(), outBuffer.begin(), ::toupper);
    return outBuffer;
}
/**
 * Return the next token contain in the expression passed to function <b>compile(Script, string</b>
 * @return
 */
string Parser::getToken() {
    currentToken = "";
    //check for endl of expression
    if (expressionIndex == (int)expression.length()){
        currentToken = "\0";
        return currentToken;
    }
    if (expressionIndex > (int)expression.length()) {
        currentToken = "\0";
        return currentToken;
    }
    //skip over white spaces
    while (expressionIndex < (int)expression.length() && isSpace(expression[expressionIndex])) {
        ++expressionIndex;
    }
    //check for endl of expression
    if (expressionIndex == (int)expression.length()) {
        currentToken = "\0";
        return currentToken;
    }
    
    //Store correct token:
    if (isDelimiter(expression[expressionIndex])) {
        currentToken.insert(currentToken.end(), 1, expression[expressionIndex]);
        expressionIndex++;
        currentTokenType = TokenType::DELIMITER;
    } else if (isLetter(expression[expressionIndex])) {
        //Grab entire word:
        while(whileNotDelimiter(expressionIndex)) {
            currentToken.insert(currentToken.end(),1,expression[expressionIndex]);
            expressionIndex++;
            if (expressionIndex >= (int)expression.length()) {
                break;
            }
        }
        
        //Set all to lower:
        string temp_currentToken = toLowerString(&currentToken);
        
        //Set this current token type to either variable or keyword
        if (isKeyword(temp_currentToken)) {
            currentTokenType = TokenType::KEYWORD;
            currentToken = temp_currentToken;
        } else {
            currentTokenType = TokenType::VAR;
        }
    } else if (isQstring(expression[expressionIndex])) {
        //Grab entire string:
        currentToken.insert(currentToken.end(),1,expression[expressionIndex]);
        expressionIndex++;
        while(!isQstring(expression[expressionIndex])) {
            if (expression[expressionIndex] == '\\') {
                currentToken.insert(currentToken.end(),1,expression[expressionIndex]);
                expressionIndex++;
                if (expressionIndex >= (int)expression.length()) { 
                    Tokens::stdError("Tokenize expression, string is not encapsulated");
                    currentToken = "\0";
                    break; 
                }
                currentToken.insert(currentToken.end(),1,expression[expressionIndex]);
                expressionIndex++;
            } else {
                currentToken.insert(currentToken.end(),1,expression[expressionIndex]);
                expressionIndex++;
            }
            if (expressionIndex >= (int)expression.length()) { 
                Tokens::stdError("Tokenize expression, string is not encapsulated");
                currentToken = "\0";
                break; 
            }
        }
        currentToken.insert(currentToken.end(),1,expression[expressionIndex]);
        expressionIndex++;
        currentTokenType = TokenType::STRING;

    } else if(isDigit(expression[expressionIndex])) {
            //Grab entire number
            while(whileNotDelimiter(expressionIndex)){ // `.` is a number too removed from the delimiter list
                currentToken.insert(currentToken.end(),1,expression[expressionIndex]);
                expressionIndex++;
                if(expressionIndex > (int)expression.length()) break;
            }
            currentTokenType = TokenType::NUMBER;
    } else {
        currentToken = "\0";
        return currentToken;
    }
    return currentToken;
}

/**
 *
 * Repeatedly call getToken as a means to populate the container class Tokens.
 *
 * Will create all the possible tokens contained in exp
 * @param exp
 * @param token
 */
void Parser::tokenize(string& exp, Tokens& token) {

    expression = exp;
    expressionIndex = 0;
    int parenthesisScaler = 0;

    //NOTE: after call to getToken
    //	    the token just created will be stored in 'currentToken'
    getToken();

    int priortyValue = 0;
    while (currentToken != "") {
        priortyValue = getDelimiterPriorty(); //get the current tokens priortyValue as a function of its evaluation hierarchy
        //this serves to ensure that multiple parenthetical grouping will be evaluated in a manner such that the deepest
        //grouping is evaluated first
        if (currentToken == string(1, Lang::LangFunctionOpenArguChar)) {
            priortyValue += parenthesisScaler;
            parenthesisScaler += 1;
        }
        
        //clean up the escaped characters of string :
        bool isEscaped = false;
        string buffToken = "";
        if (currentTokenType == TokenType::STRING) {
            for (int i=0; i<(int)currentToken.length(); i++) {
                if(currentToken[i] == '\\' && !isEscaped) {
                    isEscaped = true;
                } else if (isEscaped) {
                    switch (currentToken[i]) {
                        case 'n':
                            buffToken += '\n';
                            break;
                        case 't':
                            buffToken += '\t';
                            break;
                        case 'r':
                            buffToken += '\r';
                            break;
                        case '0':
                            buffToken += '\0';
                            break;
                        case 'b':
                            buffToken += '\b';
                            break;
                        case 'a':
                            buffToken += '\a';
                            break;
                        default:
                            buffToken += currentToken[i];
                    }
                    isEscaped = false;
                } else {
                    buffToken += currentToken[i];
                }
            }
            currentToken = buffToken;
        }
        
        token.addToken(currentToken, priortyValue, currentTokenType);
        //increase parenthesis scaler if current token was was a parenthesis

        getToken();	
        //move to next iteration of this loop
    }
}

int Parser::compiler(Script* script, Tokens& token, int rCount){
    
    if(!script) return 1;
    
    if(token.getSize() == 0) return 0;

    //assumptions:
    // parenthetically matching through out
    // syntax correct
    // parenthetical groups are handled by first case and have the highest priority
    // func/parenthetical groups have a priority of 2 -
    
    //rCount is the nesting recursive calls limit:
    rCount++;
    if (rCount > 20) return 2;

    int eraseCount = 0; //used to readjust the operation index after there is any extraction of the tokens
                        //otherwise after we extract a set of tokens, may not point to the correct location of the operator
                        //we are interested in
    int priortyCode = 0; //Lowest def.
    int operatorIndex = token.getHighestOperatorPriorityIndex(priortyCode);

    string leftToken = ""; //Look behind
    string rightToken = ""; //Look front
    string rightRightToken = "";
    if ( operatorIndex > 0 ) {
        leftToken = token.getToken( operatorIndex - 1 );
    }
    if (operatorIndex < token.getSize() && token.getSize() > 1) { 
        rightToken = token.getToken( operatorIndex + 1 );
    }
    string operatorToken = token.getToken(operatorIndex); //Current

    //If none:
    if (operatorToken == ".none.") {
        Tokens::stdError("there is no operator found");
    }
    
    //--------------------------------------------------------------------
    //Keyword has the highest priority so execute immediately
    //--------------------------------------------------------------------
    if (token.isKeyWord(operatorIndex)) {
            
        //Execute functions:
        if (operatorToken == Lang::LangFindKeyword("function")) {
            
            //add the operation opcode
            script->addInstruction( Instruction(ByteCode::FUNC, rightToken) );
            //extract what we just worked with[ functionKeyword funcName ]
            token.extractInclusive( operatorIndex, operatorIndex + 1, eraseCount );
            token.pop(operatorIndex);	//erase the RST that is placed in place of 'Function funcName'
            operatorIndex -= eraseCount;

            //all is left now is what is in '( arguments,...,argumentsN )'
            //now count the number of arguments, skip open parenthesis and close parenthesis
            int argumentCount = 0;
            if(token.getToken(token.getSize()-1) != "{"){
                Tokens::stdError("Syntax error for function definition, missing opening '{' ");
                return 3;
            } else {
                token.pop(token.getSize()-1); // erase the: '{'
            }
            
            //count the number of arguments in this parenthesis
            vector<string> arg;
            string t;
            for (int i=1; i < token.getSize()-1; i++) {
                t = token.getToken(i);
                if (t != ",") {
                    argumentCount++;
                    arg.push_back(t);
                }
            }
            
            stringstream strtoa;
            strtoa << argumentCount;
            script->addInstruction(Instruction(ByteCode::ARGC, strtoa.str()));
            for (int i=arg.size()-1; i>-1; i--) {
                script->addInstruction(Instruction(ByteCode::ARG,arg[i]));
            }
            mark(0); // 0 = function marking
            //at this point nothing else is need to be done with the arguments

            return 0;
            
        } else if (operatorToken == Lang::LangFindKeyword("cond-if")) {

            //if(expression){
            token.pop(0);	//erase the if keyword
            
            //(expression){
            if(token.getToken(token.getSize()-1) != "{") {
                Tokens::stdError("IF statement syntax error, expected a '{' after expression");
                return 4;
            } else {
                token.pop(token.getSize()-1);
                //(expression)
                mark(1); // 1- marks IF
            }
            // (expression)
            //-------------------------------------------------------
            // recursively evaluate the condition of this if statement
            //-------------------------------------------------------
            compiler(script, token, rCount);	//compile the expression
            script->addInstruction(Instruction(ByteCode::CMP));
            return 0;
        } else if (operatorToken == Lang::LangFindKeyword("loop-while")) {

            //while(expression){
            script->addInstruction(Instruction(ByteCode::LOOP));
            token.pop(0);	//erase the while keyword
            //(expression){
            if (token.getToken(token.getSize()-1) != "{") {
                Tokens::stdError("WHILE statement syntax error, expected a '{' after expression");
                return 5;
            } else {
                token.pop(token.getSize()-1);
                //(expression)
                mark(2);
            }
            // (expression)
            //------------------------------------------------------
            //recursively evaluate the condition of this while loop
            //------------------------------------------------------
            compiler(script, token, rCount); //compile the expression
            script->addInstruction(Instruction(ByteCode::CMP));
            return 0;
            
        } else if (operatorToken == Lang::LangFindKeyword("return")) {

            //extract the return value and evaluate it recursively
            if (token.getSize() > 1) {
                Tokens sub = token.extractInclusive(1,token.getSize()-1,eraseCount);
                operatorIndex -= eraseCount;
                compiler(script,sub,rCount);
            }
            script->addInstruction(Instruction(ByteCode::RET));
            return 0; //there is nothing else to be done  
        } else if (operatorToken == Lang::LangFindKeyword("variable")) {
            token.pop(operatorIndex);
            script->addInstruction(Instruction(ByteCode::DEF, rightToken));
            compiler(script, token, rCount);
            return 0;
        }
        
    }//end of keywords

    //------------------------------------------------------------
    // unmark the end of a function, if, while loop
    //------------------------------------------------------------
    if (operatorToken == "}") {
        int ret = unmark(); //unmark the grouping make earlier
        if ( ret == 2 ) { //loop
            script->addInstruction(Instruction(ByteCode::DONE,"while"));
        } else if ( ret == 1 ) {
            script->addInstruction(Instruction(ByteCode::DONE,"if"));
        }
        return 0;
    } //end of unmark


    //------------------------------------------------------
    // Handle parenthetical groupings
    //----------------------------------------------------------
    //if operator is an open parenthesis then extract the content
    if (operatorToken == "(") {
        //get the close of this parenthesis
        int closeOfParenthesis=token.getMatchingCloseParenthesis(operatorIndex);
        //extract the content and replace with RST
        Tokens sub = token.extractContentOfParenthesis(operatorIndex,closeOfParenthesis,eraseCount);
        operatorIndex-= 1;	//we extract content of parenthesis just adjust by only 1,
        compiler(script, sub,rCount);
        //Make appropriate function Call
        if (token.getSize()>1) {	//two or more
            //if previous token before the parentheis has a non zero priority of 2 then make function call
            if (token.getTokenPriorty(operatorIndex) && !isKeyword(leftToken) && !isDelimiter(leftToken)) {
                string funcName = leftToken;
                token.pop(operatorIndex); //removes function name, operatorIndex points to function name
                script->addInstruction(Instruction(ByteCode::CALL, funcName));
            }
        }
    }

    //-----------------------------------------------------------
    // Handle tokens with commas in them
    //			i.e
    //		x , 32 + y , z
    //
    //-----------------------------------------------------------
    if(hasCommas(token)){
        for (int i=0; i<token.getSize(); i++) {
            if (token.getToken(i) == ",") {
                Tokens sub = token.extractInclusive(0, i-1, eraseCount);
                operatorIndex -= eraseCount;
                compiler(script, sub,rCount);
                token.pop(0);	//remove rst
                token.pop(0);	//remove ,
                operatorIndex -= 2;
                compiler(script, token, rCount);
                break;
            }
        }
        return 0; //return do not do anything else
    }
    //---------------------------------------------------------------------------------
    //		Handle math operations in accordance with order of operations				|
    //---------------------------------------------------------------------------------
    if (priortyCode == 9) {//exponent
        script->addInstruction(Instruction(ByteCode::PUSH, leftToken));
        script->addInstruction(Instruction(ByteCode::PUSH, rightToken));
        if(token.getToken(operatorIndex)== "^") {
            script->addInstruction(Instruction(ByteCode::EXPON));
        }
        token.extractInclusive(operatorIndex-1,operatorIndex+1,eraseCount);
        operatorIndex -= eraseCount;
    } else if(priortyCode == 8) {	//multi and division
        script->addInstruction(Instruction(ByteCode::PUSH, leftToken));
        script->addInstruction(Instruction(ByteCode::PUSH, rightToken));
        if (token.getToken(operatorIndex)== "*") {
            script->addInstruction(Instruction(ByteCode::MULT));
        } else {
            script->addInstruction(Instruction(ByteCode::DIV));
        }
        token.extractInclusive(operatorIndex-1,operatorIndex+1,eraseCount);
        operatorIndex -= eraseCount;
    } else if (priortyCode == 7) {	//add and subtract
        script->addInstruction(Instruction(ByteCode::PUSH, leftToken));
        script->addInstruction(Instruction(ByteCode::PUSH, rightToken));
        if (token.getToken(operatorIndex)== "+") {
            script->addInstruction(Instruction(ByteCode::ADD));
        } else {
            script->addInstruction(Instruction(ByteCode::SUB));
        }
        token.extractInclusive(operatorIndex-1,operatorIndex+1,eraseCount);
        operatorIndex -= eraseCount;
    } else if (priortyCode == 6) {	//condition
        script->addInstruction(Instruction(ByteCode::PUSH, leftToken));
        script->addInstruction(Instruction(ByteCode::PUSH, rightToken));
        if (token.getToken(operatorIndex) == ">") {
            script->addInstruction(Instruction(ByteCode::GTR));
        } else {
            script->addInstruction(Instruction(ByteCode::LSR));
        }
        token.extractInclusive(operatorIndex-1, operatorIndex+1, eraseCount);
        operatorIndex -= eraseCount;
    } else if (priortyCode == 5) { //equal sign
            //extract from 1 past the equal sign to the end of the tokens
            Tokens sub = token.extractInclusive(operatorIndex+1,token.getSize()-1,eraseCount);
            compiler(script, sub, rCount);
            script->addInstruction(Instruction(ByteCode::ASN,leftToken));
            token.extractInclusive(operatorIndex-1,operatorIndex+1,eraseCount);
            operatorIndex -= eraseCount;
    } else if (priortyCode == 0 || priortyCode == 1) {
            script->addInstruction(Instruction(ByteCode::PUSH, token.getToken(operatorIndex)));
            token.extractInclusive(operatorIndex,operatorIndex,eraseCount);
            operatorIndex -= eraseCount;
    }

    //--------------------------------------------------------
    // Recursive compilation of whatever is not yet compiled
    //-------------------------------------------------------
    if (token.getSize() > 1) {
        return compiler(script, token, rCount);
    }
    return 0;
}

/** The compile takes a parsed expression and tokenize -> colies the tokens. 
 * 
 * @param Script script
 * @param string exp
 * @param boolean debug
 * @return int
 * 
 */
int Parser::compile(Script* script, string exp) {
    compile(script, exp, false);
}
int Parser::compile(Script* script, string exp, bool debug) { 
    if (script == NULL) {
        Tokens::stdError("compile expression, script pointer is null");
        return 1;
    }
    Tokens token;
    //generate our tokens
    tokenize(exp, token);

    //For debuging:
    if (debug) { token.renderTokens(); }
    //compile our tokens
    return compiler(script, token, 0);
}

bool Parser::whileNotDelimiter(int currentPos) {
    return currentPos < (int)expression.length() && !isDelimiter(expression[currentPos]);
}

int Parser::getDelimiterPriorty() {
    if(currentTokenType == KEYWORD) {
        return 40;
    } else if (currentToken == "}") {
        return 30;
    } else if (currentToken == "(") {
        return 10;
    } else if (currentToken == "^") {
        return 9;
    } else if (currentToken == "*" || currentToken == "/") {
        return 8;
    } else if (currentToken == "+" || currentToken=="-") {
        return 7;
    } else if (currentToken == ">" || currentToken=="<") {
        return 6;
    } else if(currentToken == "=") {
        return 5;
    } else if(currentToken == ",") {
        return -100;
    } else {
        return 0;
    }
}

/**
 * Used to describes the body of a function, conditional branches, or looping seqeunce
 *
 * markType is used to delineate between the different types of markings
 * @param markType
 * 		0 = function marking
 * 		1 = if marking
 * 		2 = while marking
 */
void Parser::mark(int markType) {
    marks.push_back(markType);
}

int Parser::unmark() {
    if(marks.size() == 0){
        Tokens::stdError("marking stack is zero unable to unmark");
        return -1;
    }
    int t = marks.back();
    marks.pop_back();
    return t;
}

bool Parser::hasCommas(Tokens& token) {
    for(int i=0; i<token.getSize(); i++){
        if(token.getToken(i) == ","){
            return true;
        }
    }
    return false;
}

Parser::~Parser() {

}

