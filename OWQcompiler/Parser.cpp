/* 
 * File:   Parser.cpp
 * Author: shlomo hassid
 *
 */
#include <iostream>
#include <stdio.h>
#include <sstream>

#include "Parser.h"

/** Construct Parser
 * 
 */
Parser::Parser() {

}
/** Destruct Parser
 * 
 */
Parser::~Parser() {

}
/** The compile takes a pre-compiled expression and -> tokenize, evaluate. 
 * 
 * @param Script script
 * @param string exp
 * @param boolean debug DEFUALT : FALSE
 * @return integer
 */
int Parser::compile(Script* script, string exp) {
    return compile(script, exp, false);
}
int Parser::compile(Script* script, string exp, bool debug) { 
    if (script == NULL) {
        Tokens::stdError("compile expression, script pointer is null");
        return 1;
    }
    Tokens tokens;
    //generate our tokens
    tokenize(exp, tokens);
    //Evaluate groups:
    evaluateGroups(tokens, TokenFlag::CONDITION);
    evaluateGroups(tokens, TokenFlag::COMPARISON);
    //For debugging:
    if (debug && OWQ_DEBUG_LEVEL > 0 && OWQ_DEBUG_EXPOSE_COMPILER_PARSE) { 
        tokens.renderTokens();
        tokens.renderTokensJoined();
    }
    
    //Reset internalStaticPointer used for RST stack calls:
    script->internalStaticPointer = 0;
    
    //compile our tokens
    return compiler(script, tokens, debug, 0);
}
/** Repeatedly call getToken as a means to populate a set of Tokens.
 *
 * @param string exp
 * @param Tokens token
 */
void Parser::tokenize(string& exp, Tokens& tokens) {
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
                if(currentToken[i] == Lang::LangStringEscape && !isEscaped) {
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
        
        //Handle negative number grouping:
        if (currentTokenType == TokenType::NUMBER && tokens.getSize() > 0) {
            //Look behind for -
            int stackTempSize = tokens.getSize();
            string behindCheck1 = tokens.getToken(stackTempSize - 1);
            if ( behindCheck1 == "-" ) {
                string behindCheck2 = tokens.getToken(stackTempSize - 2);
                bool isDelCheck2   = tokens.isDelimiter(stackTempSize - 2);
                if ((behindCheck2 != ".none" && isDelCheck2) || behindCheck2 == ".none.") {
                    //Merge with last its negative number:
                    tokens.pop(stackTempSize - 1);
                    currentToken = behindCheck1 + currentToken;
                }
            }            
        }
        tokens.addToken(currentToken, priortyValue, currentTokenType, true);
        getToken();	
    }
}
/** Return the next token contain in the expression passed to function <b>compile(Script, string</b>
 * 
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
    //First watch for double delimiters:
    if (expressionIndex + 1 < (int)expression.length()) {
        string lookInfront = string(1, expression[expressionIndex]);
        lookInfront += string(1, expression[expressionIndex+1]);
        if (isDelimiter(lookInfront)) {
            currentToken += lookInfront;
            expressionIndex += 2;
            currentTokenType = TokenType::DELIMITER;
            return currentToken;
        }
    }
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
        //Set all to lower that make sure we can check if its a keyword:
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
/** Evaluate groups in expressions mostly used to separate comparison expressions:
 * 
 * @param Tokens token
 * @param TokenFlag flagToGroup
 * @param int startFrom DEFAULT : 0
 */
void Parser::evaluateGroups(Tokens& tokens, TokenFlag flagToGroup) {
    this->evaluateGroups(tokens, flagToGroup, 0);
}
void Parser::evaluateGroups(Tokens& tokens, TokenFlag flagToGroup, int startFrom) {
    if (!tokens.setHasComparison() && !tokens.setHasCondition()) {
        return;
    }
    int setSize = tokens.getSize();
    int minPriorityCheck;
    int nestBrackets;
    int addAtPosition;
    int i;
    string braketClose = Lang::LangFindDelimiter("braketClose");
    string braketOpen = Lang::LangFindDelimiter("braketOpen");
    int bracketPriority = getDelimiterPriorty(braketOpen, TokenType::DELIMITER);
    Token* token;
    Token* checkToken;
        
    //Group flags:
    for (i = startFrom; i < setSize; i++) {
        token = tokens.getTokenObject(i);
        if (token->flag == flagToGroup) {
            minPriorityCheck = token->priority;
            tokens.pushBefore(i, braketClose, bracketPriority, TokenType::DELIMITER);
            i++;
            tokens.pushAfter(i, braketOpen, bracketPriority, TokenType::DELIMITER);
            //Scan backwards i = bracket close
            nestBrackets = 0;
            addAtPosition = 0;
            for (int l = i - 2; l >= 0; l--) {
                checkToken = tokens.getTokenObject(l);
                if (checkToken->token == braketClose) {
                    nestBrackets++;
                } else if (checkToken->token == braketOpen) {
                    if (nestBrackets > 0) {
                        nestBrackets--;
                    } else {
                        //should close
                        addAtPosition = l;
                        break;
                    }
                } else if (
                        (
                                nestBrackets == 0 
                            && checkToken->priority <= minPriorityCheck 
                            && checkToken->type == TokenType::DELIMITER
                            && checkToken->flag != flagToGroup
                        ) || (
                               nestBrackets == 0
                            && checkToken->flag == flagToGroup
                            && checkToken->priority < minPriorityCheck
                        )
                ) {
                    //should close
                    addAtPosition = l;
                    break;
                }
            }
            //close it:
            if (addAtPosition == 0) {
                tokens.pushBefore(addAtPosition, braketOpen, bracketPriority, TokenType::DELIMITER);
            } else {
                tokens.pushAfter(addAtPosition, braketOpen, bracketPriority, TokenType::DELIMITER);
            }
            //Reset pointer 
            i += 3;
            setSize += 3;
            //token.renderTokensJoined();
            //scan forward
            nestBrackets = 0;
            addAtPosition = setSize;
            for (int r = i; r < setSize; r++) {
                checkToken = tokens.getTokenObject(r);
                if (checkToken->token == braketOpen) {
                    nestBrackets++;
                } else if (checkToken->token == braketClose) {
                    if (nestBrackets > 0) {
                        nestBrackets--;
                    } else {
                        //should close
                        addAtPosition = r;
                        break;
                    }
                } else if (
                        (
                               nestBrackets == 0 
                            && checkToken->priority <= minPriorityCheck 
                            && checkToken->type == TokenType::DELIMITER
                            && checkToken->flag != flagToGroup
                        ) || (
                               nestBrackets == 0
                            && checkToken->flag == flagToGroup
                            && checkToken->priority <= minPriorityCheck
                        )
                ) {
                    //should close
                    addAtPosition = r;
                    break;
                }
            }
            //close it:
            if (addAtPosition == setSize) {
                tokens.pushAfter(addAtPosition, braketClose, bracketPriority, TokenType::DELIMITER);
            } else {
                tokens.pushBefore(addAtPosition, braketClose, bracketPriority, TokenType::DELIMITER);
            }
            //token.renderTokensJoined();
            setSize += 1;
        }
    } //Loop next
}
/** Generate the byte codes of a given Token set
 * 
 * @param Script script
 * @param Tokens token
 * @param integer rCount -> the recursion counter for limits
 * @return integer
 */
int Parser::compiler(Script* script, Tokens& tokens, bool debug, int rCount){
    
    if(!script) return 1;
    
    if(tokens.getSize() == 0) return 0;

    //assumptions:
    // parenthetically matching through out
    // syntax correct
    // parenthetical groups are handled by first case and have the highest priority
    // func/parenthetical groups have a priority of 2 -
    
    if (debug && OWQ_DEBUG_EXPOSE_COMPILER_PARSE && OWQ_DEBUG_LEVEL > 1) {
        tokens.renderTokens();
        if (OWQ_DEBUG_LEVEL > 2) { tokens.renderTokenType(); }
        if (OWQ_DEBUG_LEVEL > 3) { tokens.renderTokenPriorty(); }
        if (OWQ_DEBUG_LEVEL > 2) { Lang::printEmpLine(1); }
    }
    //rCount is the nesting recursive calls limit:
    rCount++;
    if (rCount > 20) return 2;

    int eraseCount = 0; //used to readjust the operation index after there is any extraction of the tokens
                        //otherwise after we extract a set of tokens, may not point to the correct location of the operator
                        //we are interested in
    int priortyCode = 0; //Lowest definition.
    int operatorIndex = tokens.getHighestOperatorPriorityIndex(priortyCode);
    
    string operatorTokenStr;
    Token* operatorToken;
    Token* leftToken = nullptr;
    Token* rightToken = nullptr;
    
    if ( operatorIndex > 0 ) {
        leftToken = tokens.getTokenObject( operatorIndex - 1 );
    }
    if (operatorIndex < tokens.getSize() && tokens.getSize() > 1) { 
        rightToken = tokens.getTokenObject( operatorIndex + 1 );
    }
    operatorToken = tokens.getTokenObject(operatorIndex); //Current
    operatorTokenStr = operatorToken->token;
    
    //If none:
    if (operatorToken->token == ".none.") {
        Tokens::stdError("there is no operator found");
    }
    
    //--------------------------------------------------------------------
    //Keyword has the highest priority so execute immediately
    //--------------------------------------------------------------------
    if (operatorToken->type == TokenType::KEYWORD) {

		//Validate good marks:
		if (getMark() == ParseMark::BREAKEXP) {
			return 12;
		}

        //Execute functions:
        if (rightToken != nullptr && operatorToken->token == Lang::LangFindKeyword("function")) {
            
            //add the operation opcode
            script->addInstruction(Instruction(ByteCode::FUNC, rightToken->token));
            //extract what we just worked with[ functionKeyword funcName ]
            tokens.extractInclusive( operatorIndex, operatorIndex + 1, eraseCount, script);
            tokens.pop(operatorIndex);	//erase the RST that is placed in place of 'Function funcName'
            operatorIndex -= eraseCount;

            //all is left now is what is in '( arguments,...,argumentsN )'
            //now count the number of arguments, skip open parenthesis and close parenthesis
            int argumentCount = 0;
            if(tokens.getToken(tokens.getSize()-1) != Lang::LangFindKeyword("bracesOpen")){
                Tokens::stdError("Syntax error for function definition, missing code opening " + Lang::LangFindKeyword("bracesOpen"));
                return 3;
            }
            
            tokens.pop(tokens.getSize()-1); // erase the: '{'
            
            //count the number of arguments in this parenthesis
            vector<string> arg;
            string t;
            for (int i=1; i < (int)tokens.getSize()-1; i++) {
                t = tokens.getToken(i);
                if (t != Lang::LangFindDelimiter("comma")) {
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
            mark(ParseMark::FUNCTION); // 0 = function marking
            //at this point nothing else is need to be done with the arguments

            return 0;
            
        } else if (operatorToken->token == Lang::LangFindKeyword("cond-if")) {

            //if(expression){
            tokens.pop(operatorIndex);	//erase the if keyword
            
            //(expression){
            if(tokens.getToken(tokens.getSize()-1) != Lang::LangFindDelimiter("bracesOpen")) {
                Tokens::stdError("IF statement syntax error, expected a " + Lang::LangFindDelimiter("bracesOpen"));
                return 4;
            }
            //Erase block open == brace open:
            tokens.pop(tokens.getSize()-1);
            //(expression)
            mark(ParseMark::IF); // 1- marks IF

            // (expression)
            //-------------------------------------------------------
            // recursively evaluate the condition of this if statement
            //-------------------------------------------------------
            compiler(script, tokens, debug, rCount);	//compile the expression
            script->addInstruction(Instruction(ByteCode::CMP, operatorTokenStr));
            return 0;
            
        } else if (operatorToken->token == Lang::LangFindKeyword("cond-else")) {
            //else (expression){
            tokens.pop(0);	//erase the else keyword
            //(expression){
            if(tokens.getToken(tokens.getSize()-1) != Lang::LangFindDelimiter("bracesOpen")) {
                Tokens::stdError("ELSE statement syntax error, expected a " + Lang::LangFindDelimiter("bracesOpen"));
                return 6;
            }
            tokens.pop(tokens.getSize()-1);
            //(expression)
            mark(ParseMark::ELSE); // 1- marks ELSE
            
            // (expression)
            //-------------------------------------------------------
            // recursively evaluate the condition of this if statement
            //-------------------------------------------------------
            compiler(script, tokens, debug, rCount);	//compile the expression
            script->addInstruction(Instruction(ByteCode::ELE, operatorTokenStr));
            return 0;
		}
		else if (operatorToken->token == Lang::LangFindKeyword("loop-while")) {

			//while(expression){
			script->addInstruction(Instruction(ByteCode::LOOP));
			tokens.pop(0);	//erase the while keyword
			//(expression){
			if (tokens.getToken(tokens.getSize() - 1) != Lang::LangFindDelimiter("bracesOpen")) {
				Tokens::stdError("WHILE statement syntax error, expected a " + Lang::LangFindDelimiter("bracesOpen"));
				return 5;
			}
			tokens.pop(tokens.getSize() - 1);
			//(expression)
			mark(ParseMark::WHILE);
			// (expression)
			//------------------------------------------------------
			//recursively evaluate the condition of this while loop
			//------------------------------------------------------
			compiler(script, tokens, debug, rCount); //compile the expression
			script->addInstruction(Instruction(ByteCode::CMP, operatorTokenStr));
			return 0;

		} else if (operatorToken->token == Lang::LangFindKeyword("loop-break")) {

			//Check for number of breaks:
			if (rightToken == nullptr) {
				script->addInstruction(Instruction(ByteCode::PUSH, "1"));
				script->addInstruction(Instruction(ByteCode::BRE));
				return 0;
			}
			//mark break:
			mark(ParseMark::BREAKEXP);
			//Evaluate the break expression keywords are not allowed:
			Tokens sub = tokens.extractInclusive(operatorIndex + 1, tokens.getSize() - 1, eraseCount, script);
			int ret = compiler(script, sub, debug, rCount); //compile the expression
			if (ret > 0) { return ret; }
			unmark(); //unmarks the break;
			script->addInstruction(Instruction(ByteCode::BRE));
			return 0;

        } else if (operatorToken->token == Lang::LangFindKeyword("return")) {

            //extract the return value and evaluate it recursively
            if (tokens.getSize() > 1) {
                Tokens sub = tokens.extractInclusive(1, tokens.getSize()-1, eraseCount, script);
                operatorIndex -= eraseCount;
                compiler(script, sub, debug, rCount);
            }
            script->addInstruction(Instruction(ByteCode::RET));
            return 0; //there is nothing else to be done  
		}
		else if (operatorToken->token == Lang::LangFindKeyword("variable")) {

			//Set first variable on the right:
			if (rightToken->type == TokenType::VAR) {
				script->addInstruction(Instruction(ByteCode::DEF, rightToken->token));
			} else {
				//ERROR - tried to define badly:
				return 7;
			}
			//Check for several defines: note that define key word is still in the token set.
			bool avoidEvaluation = false;
			if (hasCommasNotNested(tokens)) {
				//TODO avoid BRACKETS!
				//Scan to Define all recursivly:
				string t;
				string langCommas = Lang::LangFindDelimiter("comma");
				string langOpenBracket = Lang::LangFindDelimiter("braketOpen");
				string langCloseBracket = Lang::LangFindDelimiter("braketClose");
				int tokenSetSize = (int)tokens.getSize();
				int nestedBrackets = 0;
				for (int i = operatorIndex + 1; i < tokenSetSize - 1; i++) {
					t = tokens.getToken(i);
					if (t == langOpenBracket) {
						nestedBrackets++;
					} else if (t == langCloseBracket) {
						nestedBrackets--;
					} else if (t == langCommas && nestedBrackets == 0) {
						Tokens sub = tokens.extractInclusive(operatorIndex + 1, i - 1, eraseCount, script);
						i -= eraseCount;
						tokenSetSize -= eraseCount;
						//If the sub is just one token avoid anything elese and continue:
						if (sub.getSize() > 1) {
							//Evaluate:
							int t1 = evaluateDeclarationSub(sub, true);
							if (t1 > 0) { return t1; } //Invalid return error code!
							//Compile sub expression:
							compiler(script, sub, debug, rCount);
						}
						tokens.pop(i); //Pop RST || variable
						tokens.pop(i); //Pop comma
						avoidEvaluation = true;
						break;
					}

				}
			} else {
				//This removes the define key word
				tokens.pop(operatorIndex);
			}
			//Evaluate and check if there is equal sign:
			if (!avoidEvaluation) {
				if (evaluateDeclarationSub(tokens, false) > 0) {
					tokens.renderTokens();
					return 9;
				}
			}
			//Continue compilation:
			if (tokens.getSize() > 1) {
				compiler(script, tokens, debug, rCount);
			} 
            return 0;
        }
    } //end of keywords

    //------------------------------------------------------------
    // un-mark the end of a function, if, while loop
    //------------------------------------------------------------
    if (operatorToken->token == Lang::LangFindDelimiter("bracesClose")) {
        ParseMark ret = unmark(); //un-mark the grouping make earlier
        if ( ret == ParseMark::WHILE ) { //loop
            script->addInstruction(Instruction(ByteCode::DONE, Lang::LangFindKeyword("loop-while")));
        } else if ( ret == ParseMark::IF ) {
            script->addInstruction(Instruction(ByteCode::DONE, Lang::LangFindKeyword("cond-if")));
        } else if ( ret == ParseMark::ELSE ) {
            script->addInstruction(Instruction(ByteCode::DONE, Lang::LangFindKeyword("cond-else")));
		} else {
			//Bad palcement of the barces
			return 8;
		}
        return 0;
    } //end of un-mark

    //------------------------------------------------------
    // Handle parenthetical groupings
    //----------------------------------------------------------
    //if operator is an open parenthesis then extract the content
    if (operatorToken->token == Lang::LangFindDelimiter("braketOpen")) { //  bracket (
        //get the close of this parenthesis

        int closeOfParenthesis = tokens.getMatchingCloseParenthesis(operatorIndex);
        //extract the content and replace with RST
        Tokens sub = tokens.extractContentOfParenthesis(operatorIndex, closeOfParenthesis, eraseCount, script);
        operatorIndex -= 1;	//Just in case its a function call set next block to parse the call name,
        int prevRstPos = script->internalStaticPointer; //just in case the group won't do anything but push
        compiler(script, sub, debug, rCount);
        
        //Set the correct Static Pointer of the brackets group since we set it to zero by default:
        if (tokens.getSize() > operatorIndex + 1) {
			if (prevRstPos == script->internalStaticPointer) {
				script->internalStaticPointer++;
			}
            tokens.tokens[operatorIndex + 1].rstPos = script->internalStaticPointer;
        }
        
        //Make appropriate function Call
        if (tokens.getSize() > 1 && operatorIndex >= 0 && leftToken != nullptr) { //two or more
            //if previous token before the parenthesis has a non zero priority of 2 then make function call
            if (tokens.getTokenPriorty(operatorIndex) && leftToken->type != TokenType::KEYWORD && leftToken->type != TokenType::DELIMITER) {
                string funcName = leftToken->token;
				script->addInstruction(Instruction(ByteCode::CALL, funcName, tokens.tokens[operatorIndex + 1].rstPos));
                tokens.pop(operatorIndex); //removes function name, operatorIndex points to function name
                
            }
        }
    }

    //-----------------------------------------------------------
    // Handle tokens with commas in them
    //			i.e
    //		x , 32 + y , z
    //
    //-----------------------------------------------------------
    if (hasCommas(tokens)) {
        for (int i = 0; i<tokens.getSize(); i++) {
            if (tokens.getToken(i) == Lang::LangFindDelimiter("comma")) {
                Tokens sub = tokens.extractInclusive(0, i - 1, eraseCount, script);
                operatorIndex -= eraseCount;
                compiler(script, sub, debug, rCount);
                tokens.pop(0);	//remove RST
                tokens.pop(0);	//remove comma
                operatorIndex -= 2;
                compiler(script, tokens, debug, rCount);
                break;
            }
        }
        return 0; //return do not do anything else
    }
    //---------------------------------------------------------------------------------
    //		Handle math operations in accordance with order of operations
    //---------------------------------------------------------------------------------
    
    //exponent ^
    if (priortyCode == 90) { 

        compile_LR_mathLogigBaseOperations(ByteCode::EXPON, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
    
    //multi and division 
    } else if (priortyCode == 80) {	
    
        if (operatorToken->token == Lang::LangFindDelimiter("multi")) { //Multiple
            compile_LR_mathLogigBaseOperations(ByteCode::MULT, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { //Divide
            compile_LR_mathLogigBaseOperations(ByteCode::DIV, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        }
        
    //add and subtract
    } else if (priortyCode == 70) { 
    
        if (operatorToken->token == Lang::LangFindDelimiter("plus")) { // ADD values 
            compile_LR_mathLogigBaseOperations(ByteCode::ADD, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { // Subtract values
            compile_LR_mathLogigBaseOperations(ByteCode::SUB, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        }
    //greater lesser
    } else if (priortyCode == 60) { 
        
        if (operatorToken->token == Lang::LangFindDelimiter("greater")) { // Is greater than
            compile_LR_mathLogigBaseOperations(ByteCode::GTR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { // Is smaller than
            compile_LR_mathLogigBaseOperations(ByteCode::LSR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        }
        
    //c-equals ==
    } else if (priortyCode == 59) {
        
        if (operatorToken->token == Lang::LangFindDelimiter("c-equal")) { // Is Equal to
            compile_LR_mathLogigBaseOperations(ByteCode::CVE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { //
            
        }
        
    // matching signs logics
    } else if (priortyCode == 50 || priortyCode == 49) {
        
        if (operatorToken->token == Lang::LangFindDelimiter("and")) { // Is LOGIC AND
            compile_LR_mathLogigBaseOperations(ByteCode::AND, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { // Is LOGIC OR
            compile_LR_mathLogigBaseOperations(ByteCode::POR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        }
        
    } else if (priortyCode == 40) { //equal sign

        //extract from 1 past the equal sign to the end of the tokens
        Tokens sub = tokens.extractInclusive(operatorIndex+1, tokens.getSize()-1, eraseCount, script);
        compiler(script, sub, debug, rCount);
        script->addInstruction(Instruction(ByteCode::ASN, leftToken->token));
        tokens.extractInclusive(operatorIndex-1, operatorIndex+1, eraseCount, script);
        operatorIndex -= eraseCount;
            
    } else if (priortyCode == 0 || priortyCode == 1) {
		script->addInstruction(Instruction(ByteCode::PUSH, tokens.getToken(operatorIndex)));
		if (operatorIndex == tokens.getSize() - 1) {
			tokens.extractInclusive(operatorIndex, operatorIndex, eraseCount, script, true);
		}
		else {
			tokens.extractInclusive(operatorIndex, operatorIndex, eraseCount, script);
		}
		operatorIndex -= eraseCount;
    }
    //--------------------------------------------------------
    // Recursive compilation of whatever is not yet compiled
    //-------------------------------------------------------
    if (tokens.getSize() > 1) {
        return compiler(script, tokens, debug, rCount);
    }
    
    return 0;
}
/** Used to describes the body of a function, conditional branches, or looping
 *
 * markType is used to delineate between the different types of markings
 * @param markType
 *    0 = function marking
 *    1 = if marking
 *    2 = while marking
 *    3 = else
 */
void Parser::mark(ParseMark markType) {
    marks.push_back(markType);
}
ParseMark Parser::unmark() {
    if (marks.size() == 0) {
        Tokens::stdError("marking stack is zero unable to unmark");
        return ParseMark::UNMARK;
    }
    ParseMark t = marks.back();
    marks.pop_back();
    return t;
}
ParseMark Parser::getMark() {
	if (marks.size() == 0) {
		return ParseMark::UNMARK;
	}
	return marks.back();
}
/** Generate byte-code for Math and logic based operations
 * 
 * @param ByteCode bc
 * @param Script* script
 * @param Token* token
 * @param integer operatorIndex
 * @param integer eraseCount
 * @param string leftToken
 * @param string rightToken
 * @return boolean
 */
bool Parser::compile_LR_mathLogigBaseOperations(ByteCode bc, Script*& script, Tokens* tokens, int &operatorIndex, int &priority, int &eraseCount, Token* leftToken, Token* rightToken) {

    script->addInstruction(Instruction(ByteCode::PUSH, leftToken->token, leftToken->rstPos), true);
    script->addInstruction(Instruction(ByteCode::PUSH, rightToken->token, rightToken->rstPos), true);
    script->addInstruction(Instruction(bc));
    tokens->extractInclusive(operatorIndex - 1, operatorIndex + 1, eraseCount, script, true);
    operatorIndex -= eraseCount;
    return true;
}




int Parser::evaluateDeclarationSub(Tokens &sub, bool andTypes) {
	int size = sub.getSize();
	Token* token = nullptr;
	for (int i = 0; i < size; i++ ) {
		token = sub.getTokenObject(i);
		switch (i) {
		case 0:
			if (token->token == Lang::LangFindDelimiter("comma")) {
				return 10;
			}
			break;
		case 1:
			if (token->token != Lang::LangFindDelimiter("equal")) {
				return 9;
			}
			break;
		default:
			if (!andTypes) { return 0; }
			if (token->type == TokenType::KEYWORD) {
				return 11;
			}
		}
	}
	return 0;
}
bool Parser::evaluateSetIncludeType(Tokens &sub, TokenType type) {
	int size = sub.getSize();
	Token* token = nullptr;
	for (int i = 0; i < size; i++) {
		token = sub.getTokenObject(i);
		if (token->type == type) {
			return true;
		}
	}
	return false;
}




/** loop condition to find all until delimiter
 * @param integer currentPos
 * @return boolean
 */
bool Parser::whileNotDelimiter(int currentPos) {
    return currentPos < (int)expression.length() && !isDelimiter(expression[currentPos]);
}
/** Indicate whether or not a char constant c is one of our defined delimiters
 *  Note: delimiters are used to separate individual tokens
 * 
 * @param string|char c
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
 * @param char|string c
 * @return boolean
 */
bool Parser::isSpace(const char& c) {
    return (string(1, c) == Lang::LangFindDelimiter("space")) ? true : false;
}
bool Parser::isSpace(const string& c) {
    return (c == Lang::LangFindDelimiter("space")) ? true : false;
}
/** Check whether we entering using a string:
 * 
 * @param char|string c
 * @return boolean
 */
bool Parser::isQstring(const char& c) {
    return c == Lang::LangStringIndicator;
}
bool Parser::isQstring(const string& c) {
    return c[0] == Lang::LangStringIndicator;
}
/** Validate naming chars:
 * 
 * @param char|string c
 * @return boolean
 */
bool Parser::isLetter(const char& c) {
    return Lang::LangIsNamingAllowed(c);
}
bool Parser::isLetter(const string& c) {
    return Lang::LangIsNamingAllowed(c);
}
/** Indicate true of false if a specific character constant, c, is a digit or not
 * 
 * @param char|string c
 * @return boolean
 */
bool Parser::isDigit(const string& c) {
	return isDigit(c[0]);
}
bool Parser::isDigit(const char& c) {
	bool ret = false;
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
/** Indicate if supplied string, s, is a keyword
 * 
 * @param string s
 * @return boolean
 */
bool Parser::isKeyword(string s) {
    if (Lang::LangIsKeyword(s)) {
        currentTokenType = TokenType::KEYWORD;
        return true;
    }
    return false;
}
/** Translates an delimiter string to a Precedence Priority
 * 
 * @return integer
 */
int Parser::getDelimiterPriorty() {
    if(currentTokenType == KEYWORD) {
        return 140;
    } else if (currentToken == Lang::LangFindDelimiter("bracesClose")) {
        return 130;
    } else if (currentToken == Lang::LangFindDelimiter("braketOpen")) {
        return 110;
    } else if (currentToken == Lang::LangFindDelimiter("power")) {
        return 90;
    } else if (currentToken == Lang::LangFindDelimiter("multi") || currentToken == Lang::LangFindDelimiter("divide")) {
        return 80;
    } else if (currentToken == Lang::LangFindDelimiter("plus") || currentToken == Lang::LangFindDelimiter("minus")) {
        return 70;
    } else if (currentToken == Lang::LangFindDelimiter("smaller") || currentToken == Lang::LangFindDelimiter("greater")) {
        return 60;
    } else if (currentToken == Lang::LangFindDelimiter("c-equal")) {
        return 59;
    } else if (currentToken == Lang::LangFindDelimiter("and")) {
        return 50;
    } else if (currentToken == Lang::LangFindDelimiter("or")) {
        return 49;
    } else if (currentToken == Lang::LangFindDelimiter("equal")) {
        return 40;
    }  else if (currentToken == Lang::LangFindDelimiter("comma")) {
        return -100;
    } else {
        return 0;
    }
}
/** Translates an delimiter string to a Precedence Priority
 * 
 * @param string toCheckToken
 * @param TokenType toCheckType
 * @return integer
 */
int Parser::getDelimiterPriorty(string toCheckToken, TokenType toCheckType) {
    if(toCheckType == KEYWORD) {
        return 140;
    } else if (toCheckToken == Lang::LangFindDelimiter("bracesClose")) {
        return 130;
    } else if (toCheckToken == Lang::LangFindDelimiter("braketOpen")) {
        return 110;
    } else if (toCheckToken == Lang::LangFindDelimiter("power")) {
        return 90;
    } else if (toCheckToken == Lang::LangFindDelimiter("multi") || toCheckToken == Lang::LangFindDelimiter("divide")) {
        return 80;
    } else if (toCheckToken == Lang::LangFindDelimiter("plus") || toCheckToken == Lang::LangFindDelimiter("minus")) {
        return 70;
    } else if (toCheckToken == Lang::LangFindDelimiter("smaller") || toCheckToken == Lang::LangFindDelimiter("greater")) {
        return 60;
    } else if (toCheckToken == Lang::LangFindDelimiter("c-equal")) {
        return 59;
    } else if (toCheckToken == Lang::LangFindDelimiter("and")) {
        return 50;
    } else if (toCheckToken == Lang::LangFindDelimiter("or")) {
        return 49;
    } else if (toCheckToken == Lang::LangFindDelimiter("equal")) {
        return 40;
    }  else if (toCheckToken == Lang::LangFindDelimiter("comma")) {
        return -100;
    } else {
        return 0;
    }
}
/** Check to see if a token group has commas in it 
 * 
 * @param Tokens token  -> the entire Token set
 * @return boolean
 */
bool Parser::hasCommas(Tokens& tokens) {
    int size = tokens.getSize();
	string comma = Lang::LangFindDelimiter("comma");
    for (int i = 0; i < size; i++){
        if(tokens.getToken(i) == comma){
            return true;
        }
    }
    return false;
}
/** Check to see if a token group has commas in it but avoid nested (, , ,)
*
* @param Tokens token  -> the entire Token set
* @return boolean
*/
bool Parser::hasCommasNotNested(Tokens& tokens) {
	int size = tokens.getSize();
	string comma = Lang::LangFindDelimiter("comma");
	string langOpenBracket = Lang::LangFindDelimiter("braketOpen");
	string langCloseBracket = Lang::LangFindDelimiter("braketClose");
	int nested = 0;
	for (int i = 0; i < size; i++) {
		string t = tokens.getToken(i);
		if (t == langOpenBracket) {
			nested++;
		}
		else if (t == langCloseBracket && nested > 0) {
			nested--;
		}
		if (nested < 1 && t == comma) {
			return true;
		}
	}
	return false;
}
/** Parse any string to lower ASCII chars
 * 
 * @param string s
 * @return string
 */
string Parser::toLowerString(string *s) {
    string outBuffer = *s;
    transform(s->begin(), s->end(), outBuffer.begin(), ::tolower);
    return outBuffer;
}
/** Parse any string to lower ASCII chars
 * 
 * @param string s
 * @return string
 */
string Parser::toUpperString(string *s) {
    string outBuffer = *s;
    transform(s->begin(), s->end(), outBuffer.begin(), ::toupper);
    return outBuffer;
}