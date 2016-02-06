/* 
 * File:   Parser.cpp
 * Author: shlomo hassid
 *
 */
#include <iostream>
#include <stdio.h>
#include <sstream>

#include "Parser.h"

namespace Eowq {
std::string Parser::errors[] = {
	"",
	"1 script object is null",
	"2 recursive call max out script contains error",
	"3 syntax error for function definition",
	"4 if - statement syntax error",
	"5 while - statement syntax error",
	"6 else - statement syntax error",
	"7 Definition - expected definition of valriable name",
	"8 Missuse of Braces",
	"9 Declaration of variables should be followed by an assignment delimiter or by end of statement",
	"10 Found two commas in variable declaration",
	"11 Declaraion expression cannot contain keywords",
	"12 break statement can't use keywords",
	"13 Function declaration is expecting argument brackets followed by brace open char.",
	"14 Braces are not allowed in a argument of functions - missing a bracket close?",
	"15 Keywords are not allowed in as function arguments - missing a bracket close?",
	"16 Braces should be use after function argument brackets.",
	"17 Unexpected bracket close - missing a bracket open char?",
	"18 Function declaratin is missing a function name.",
	"19 Unset - unset expression is not legal you should unset only variables.",
	"20 Missuse of Increment / Decrement operator - should be attached to a variable name only.",
	"21 Keywords are not allowed inside Array square brackets",
	"22 Assignments operators and block delimiters are not allowedinside Array square brackets"
};

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
int Parser::compile(Script* script, std::string exp) {
    return compile(script, exp, false);
}
int Parser::compile(Script* script, std::string exp, bool debug) {
    if (script == NULL) {
		Tokens::stdError("compile expression, script pointer is null");
        return 1;
    }
	Tokens tokens;
	int ret = 0;
    //generate our tokens
    tokenize(exp, tokens);

	//Loop from sub expression (;) may be passed by macros:
	while (tokens.getSize() > 0) {
		//Sub exp
		Tokens subexp = tokens.extractSubExpr();
		if (subexp.getSize() < 1) {
			continue;
		}
		//Evaluate groups:
		evaluateGroups(subexp, TokenFlag::CONDITION);
		evaluateGroups(subexp, TokenFlag::COMPARISON);
		//For debugging:
		if (debug && OWQ_DEBUG_LEVEL > 0 && OWQ_DEBUG_EXPOSE_COMPILER_PARSE) {
			subexp.renderTokens();
			subexp.renderTokensJoined();
		}

		//Reset internalStaticPointer used for RST stack calls:
		script->internalStaticPointer = 0;

		ret = compiler(script, subexp, debug, 0);
		if (ret > 0) { break; }
	}
    //compile our tokens
    return ret;
}
/** Repeatedly call getToken as a means to populate a set of Tokens.
 *
 * @param string exp
 * @param Tokens token
 */
void Parser::tokenize(std::string& exp, Tokens& tokens) {
    expression = exp;
    expressionIndex = 0;
    int parenthesisScaler = 0;
	int braketScaler = 0;
    //NOTE: after call to getToken
    //	    the token just created will be stored in 'currentToken'
    getToken();

    int priortyValue = 0;
    while (currentToken != "") {
        priortyValue = getDelimiterPriorty(); //get the current tokens priortyValue as a function of its evaluation hierarchy
        //this serves to ensure that multiple parenthetical grouping will be evaluated in a manner such that the deepest
        //grouping is evaluated first
        if (currentToken[0] == Lang::LangFunctionOpenArguChar) {
            priortyValue += parenthesisScaler++;
        } else if (currentToken[0] == Lang::LangArrayOpenChar) {
			priortyValue += braketScaler++;
		}

        //clean up the escaped characters of string :
        bool isEscaped = false;
		std::string buffToken = "";
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
			std::string behindCheck1 = tokens.getToken(stackTempSize - 1);
            if ( behindCheck1 == "-" ) {
				std::string behindCheck2 = tokens.getToken(stackTempSize - 2);
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
std::string Parser::getToken() {
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
    //skip over white spaces and ;
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
		std::string lookInfront;
		lookInfront += expression[expressionIndex];
        lookInfront += expression[expressionIndex+1];
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
		std::string temp_currentToken = toLowerString(&currentToken);
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
	std::string braketClose = Lang::dicLang_braketClose;
	std::string braketOpen = Lang::dicLang_braketOpen;
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
    
	std::string operatorTokenStr;
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
        if (rightToken != nullptr && operatorToken->token == Lang::dicLangKey_function) {
            
			//Throw error if right token is not a name:
			if (rightToken->type != TokenType::VAR) {
				return 18;
			}
            //add the operation opcode
            script->addInstruction(Instruction(ByteCode::FUNC, rightToken->token));
            //extract what we just worked with[ functionKeyword funcName ]
            tokens.extractInclusive( operatorIndex, operatorIndex + 1, eraseCount, script);
			tokens.renderTokens();
            tokens.pop(operatorIndex);	//erase the RST that is placed in place of 'Function funcName'
            operatorIndex -= eraseCount;
			tokens.renderTokens();
            //all is left now is what is in '( arguments,...,argumentsN )'
            //now count the number of arguments, skip open parenthesis and close parenthesis
            
			int evaluate = evaluateFunctionDeclaration(tokens);
			if (evaluate > 0) {
				return evaluate;
			}
			int argumentCount = 0;

            
            
            tokens.pop(tokens.getSize()-1); // erase the: '{'
            
            //count the number of arguments in this parenthesis
			std::vector<std::string> arg;
			std::string t;
            for (int i=1; i < (int)tokens.getSize()-1; i++) {
                t = tokens.getToken(i);
                if (t != Lang::dicLang_comma) {
                    argumentCount++;
                    arg.push_back(t);
                }
            }
            
			std::stringstream strtoa;
            strtoa << argumentCount;
            script->addInstruction(Instruction(ByteCode::ARGC, strtoa.str()));
            for (int i=arg.size()-1; i>-1; i--) {
                script->addInstruction(Instruction(ByteCode::ARG,arg[i]));
            }
            mark(ParseMark::FUNCTION); // 0 = function marking
            //at this point nothing else is need to be done with the arguments

            return 0;
            
        } else if (operatorToken->token == Lang::dicLangKey_cond_if) {

            //if(expression){
            tokens.pop(operatorIndex);	//erase the if keyword
            
            //(expression){
            if(tokens.getToken(tokens.getSize()-1) != Lang::dicLang_bracesOpen) {
				Tokens::stdError("IF statement syntax error, expected a " + Lang::dicLang_bracesOpen);
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
            
        } else if (operatorToken->token == Lang::dicLangKey_cond_else) {
            //else (expression){
            tokens.pop(0);	//erase the else keyword
            //(expression){
            if(tokens.getToken(tokens.getSize()-1) != Lang::dicLang_bracesOpen) {
				Tokens::stdError("ELSE statement syntax error, expected a " + Lang::dicLang_bracesOpen);
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
		else if (operatorToken->token == Lang::dicLangKey_loop_while) {

			//while(expression){
			script->addInstruction(Instruction(ByteCode::LOOP, operatorTokenStr));
			tokens.pop(0);	//erase the while keyword
			//(expression){
			if (tokens.getToken(tokens.getSize() - 1) != Lang::dicLang_bracesOpen) {
				Tokens::stdError("WHILE statement syntax error, expected a " + Lang::dicLang_bracesOpen);
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

		} else if (operatorToken->token == Lang::dicLangKey_loop_break) {

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

        } else if (operatorToken->token == Lang::dicLangKey_cond_break) {

			//Check for number of breaks:
			if (rightToken == nullptr) {
				script->addInstruction(Instruction(ByteCode::PUSH, "1"));
				script->addInstruction(Instruction(ByteCode::BIF));
				return 0;
			}
			//mark break same as loop break:
			mark(ParseMark::BREAKEXP);
			//Evaluate the break expression keywords are not allowed:
			Tokens sub = tokens.extractInclusive(operatorIndex + 1, tokens.getSize() - 1, eraseCount, script);
			int ret = compiler(script, sub, debug, rCount); //compile the expression
			if (ret > 0) { return ret; }
			unmark(); //unmarks the break;
			script->addInstruction(Instruction(ByteCode::BIF));
			return 0;

		} else if (operatorToken->token == Lang::dicLangKey_return) {

            //extract the return value and evaluate it recursively
            if (tokens.getSize() > 1) {
				Tokens sub = tokens.extractInclusive(1, tokens.getSize()-1, eraseCount, script);
                operatorIndex -= eraseCount;
                compiler(script, sub, debug, rCount);
            }
            script->addInstruction(Instruction(ByteCode::RET));
            return 0; //there is nothing else to be done  
		} else if (operatorToken->token == Lang::dicLangKey_variable) {

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
				std::string t;
				std::string langCommas = Lang::dicLang_comma;
				std::string langOpenBracket = Lang::dicLang_braketOpen;
				std::string langCloseBracket = Lang::dicLang_braketClose;
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
				return compiler(script, tokens, debug, rCount);
			} 
            return 0;

		} else if (operatorToken->token == Lang::dicLangKey_unset) {
			//Unset first variable on the right:
			if (rightToken != nullptr && rightToken->type == TokenType::VAR) {
				if (evaluateVarNotObjectCall(rightToken)) {
					script->addInstruction(Instruction(ByteCode::UNS, rightToken->token));
				} else {
					return 19;
				}
			} else {
				//ERROR - tried to unset badly:
				return 7;
			}
			//This removes the define key word and the unset variable:
			tokens.pop(operatorIndex);
			tokens.pop(operatorIndex);
			//Scan to allow bulk unset:
			if (hasCommasNotNested(tokens)) {
				//Scan to Unset all:
				Token* t;
				int tokenSetSize = (int)tokens.getSize();
				for (int i = operatorIndex; i < tokenSetSize; i++) {
					t = tokens.getTokenObject(i);
					if (t->token == Lang::dicLang_comma) { continue; }
					if (t->type != TokenType::VAR) { return 19; }
					if (!evaluateVarNotObjectCall(t)) { return 19; }
					script->addInstruction(Instruction(ByteCode::UNS, t->token));
				}
			}
			return 0;
		}
    } //end of keywords

    //------------------------------------------------------------
    // un-mark the end of a function, if, while loop
    //------------------------------------------------------------
    if (operatorToken->token == Lang::dicLang_bracesClose) {
		ParseMark ret = unmark(); //un-mark the grouping make earlier
        if ( ret == ParseMark::WHILE ) { //loop
            script->addInstruction(Instruction(ByteCode::DONE, Lang::dicLangKey_loop_while));
        } else if ( ret == ParseMark::IF ) {
            script->addInstruction(Instruction(ByteCode::DONE, Lang::dicLangKey_cond_if));
        } else if ( ret == ParseMark::ELSE ) {
            script->addInstruction(Instruction(ByteCode::DONE, Lang::dicLangKey_cond_else));
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
    if (operatorToken->token == Lang::dicLang_braketOpen) { //  bracket (

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
				if (operatorIndex == 0 && rCount == 1) { // A garbage preventor:
					script->addInstruction(Instruction(ByteCode::DPUSH, "CALL"));
				}
				std::string funcName = leftToken->token;
				script->addInstruction(Instruction(ByteCode::CALL, funcName, tokens.tokens[operatorIndex + 1].rstPos));
                tokens.pop(operatorIndex); //removes function name, operatorIndex points to function name
                
            }
        }
    }
	
    //-----------------------------------------------------------
    // Handle tokens with commas in them
    //			i.e
    //		x , 32 + y , z
    // will seperate them into subs and re-compile:
    //-----------------------------------------------------------
    if (hasCommasNotNested(tokens)) {
		int commaIndex = getCommaIndexNotNested(tokens);
		Tokens sub = tokens.extractInclusive(0, commaIndex, eraseCount, script);
		sub.pop(commaIndex);	//remove comma
		tokens.pop(0);		    //remove rst
        operatorIndex -= eraseCount;
		if (sub.getSize() == 1) {
			Token* examineToken = sub.getTokenObject(0);
			if (examineToken->type == TokenType::RST || examineToken->type == TokenType::NUMBER || examineToken->type == TokenType::STRING || examineToken->type == TokenType::VAR) {
				script->addInstruction(Instruction(ByteCode::PUSH, sub.getTokenObject(0)->token, sub.getTokenObject(0)->rstPos), true);
			} else {
				//Probably an error...
			}
		} else {
			compiler(script, sub, debug, rCount);
		}

        return compiler(script, tokens, debug, rCount); //return do not do anything else
    }

	//-----------------------------------------------------------
	// Handle square brackets
	//-----------------------------------------------------------
	if (operatorToken->token == Lang::dicLang_sBraketOpen) { //  bracket [
															 //get the close of this bracket square
		int closeOfSquareBrackets = tokens.getMatchingCloseSquareBrackets(operatorIndex);
		//extract the content and replace with RST
		Tokens sub = tokens.extractContentOfParenthesis(operatorIndex, closeOfSquareBrackets, eraseCount, script);

		//If its the first extract then move on (avoids garbage):

		//Evaluate sub tokens of array call:
		int evSBres = evaluateArraySbrackets(sub);
		if (evSBres > 0) { return evSBres == 1 ? 22 : 21; }
		
		//Count comma cells:
		int arrayElementsCount = countCommasNotNested(sub);

		//This will indicate an array push flag the left token, remove rst and continue:
		Token* leftOverLook = tokens.tokenLeftLookBeforeArrayTraverse(operatorIndex);
		if (arrayElementsCount == 0 && leftOverLook != nullptr && leftOverLook->type == TokenType::VAR) {
			leftOverLook->setArrayTreatPush(true);
			tokens.pop(operatorIndex); //Pop RST 
		//This will indicate an array traverse callee:
		} else if (leftOverLook != nullptr && leftOverLook->type == TokenType::VAR) {
			leftOverLook->setArrayTraverse(arrayElementsCount);
			int ret = compiler(script, sub, debug, rCount);
			leftOverLook->setArrayPathStaticPointer(script->code.back().getPointer());
			tokens.pop(operatorIndex); //Pop RST 
			if (ret != 0) {
				//An array error.
				return ret;
			}
		} else {
			//Probably a definition

			//Parse elements:
			int ret = 0;
			if (arrayElementsCount > 0) {
				// compile sub:
				ret = compiler(script, sub, debug, rCount);
			}

			//Rst pointer sync:
			if (tokens.getTokenObject(operatorIndex)->rstPos < script->internalStaticPointer) {
				tokens.getTokenObject(operatorIndex)->rstPos = script->internalStaticPointer++;
			}
			//If everything is fine add the constructor instructions:
			if (ret == 0) {
				std::stringstream strtoa;
				strtoa << arrayElementsCount;
				int test = tokens.getTokenObject(operatorIndex)->rstPos;
				script->addInstruction(Instruction(ByteCode::ARD, strtoa.str(), tokens.getTokenObject(operatorIndex)->rstPos), true);
			} else {
				return ret;
			}
		}

		return compiler(script, tokens, debug, rCount);
	}

    //---------------------------------------------------------------------------------
    //		Handle math operations in accordance with order of operations
    //---------------------------------------------------------------------------------
    
	//increment decrement ++ --
	if (priortyCode == 91) {
		if (leftToken != nullptr && ( leftToken->type == TokenType::VAR)) {
			//Is of type Postfix
			if (operatorIndex == 1 && rCount == 1) { // A garbage preventor:
				script->addInstruction(Instruction(ByteCode::DPUSH, operatorToken->token));
			}
			if (operatorToken->token == Lang::dicLang_dec) {
				script->addInstruction(Instruction(ByteCode::DECL, leftToken->token, leftToken->rstPos), true);
			} else {
				script->addInstruction(Instruction(ByteCode::INCL, leftToken->token, leftToken->rstPos), true);
				
			}
			tokens.extractInclusive(operatorIndex - 1, operatorIndex, eraseCount, script, true);
		} else if (rightToken != nullptr && (rightToken->type == TokenType::VAR)) {
			//Is of type Prefix
			if (operatorIndex == 0) { // A garbage preventor:
				script->addInstruction(Instruction(ByteCode::DPUSH, operatorToken->token));
			}
			if (operatorToken->token == Lang::dicLang_dec) {
				script->addInstruction(Instruction(ByteCode::DECR, rightToken->token, rightToken->rstPos), true);
			} else {
				script->addInstruction(Instruction(ByteCode::INCR, rightToken->token, rightToken->rstPos), true);

			}
			tokens.extractInclusive(operatorIndex, operatorIndex + 1, eraseCount, script, true);
		} else {
			//Error with increment decrement operator:
			return 20;
		}
		operatorIndex -= eraseCount;
	//exponent ^
	} else if (priortyCode == 90) { 

        compile_LR_mathLogigBaseOperations(ByteCode::EXPON, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
    
    //multi and division 
    } else if (priortyCode == 80) {	
    
        if (operatorToken->token == Lang::dicLang_multi) { //Multiple
            compile_LR_mathLogigBaseOperations(ByteCode::MULT, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { //Divide
            compile_LR_mathLogigBaseOperations(ByteCode::DIV, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        }
        
    //add and subtract
    } else if (priortyCode == 70) { 
    
        if (operatorToken->token == Lang::dicLang_plus) { // ADD values 
            compile_LR_mathLogigBaseOperations(ByteCode::ADD, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { // Subtract values
            compile_LR_mathLogigBaseOperations(ByteCode::SUB, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        }
    //greater lesser
    } else if (priortyCode == 60) { 
        
        if (operatorToken->token == Lang::dicLang_greater) { // Is greater than
            compile_LR_mathLogigBaseOperations(ByteCode::GTR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else if (operatorToken->token == Lang::dicLang_smaller) { // Is smaller than
            compile_LR_mathLogigBaseOperations(ByteCode::LSR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else if (operatorToken->token == Lang::dicLang_greater_equal) { // Is greater or equal to
			compile_LR_mathLogigBaseOperations(ByteCode::GTRE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
		} else if (operatorToken->token == Lang::dicLang_smaller_equal) { // Is smaller or equal to
			compile_LR_mathLogigBaseOperations(ByteCode::LSRE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
		}
        
    //c-equals ==
    } else if (priortyCode == 59) {
        
        if (operatorToken->token == Lang::dicLang_c_equal) { // Is value Equal to
            compile_LR_mathLogigBaseOperations(ByteCode::CVE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else if (operatorToken->token == Lang::dicLang_c_nequal){ // Is value not Equal to
			compile_LR_mathLogigBaseOperations(ByteCode::CVN, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else if (operatorToken->token == Lang::dicLang_c_tequal) { // Is type Equal to
			compile_LR_mathLogigBaseOperations(ByteCode::CTE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
		} else if (operatorToken->token == Lang::dicLang_c_ntequal) { // Is type not Equal to
			compile_LR_mathLogigBaseOperations(ByteCode::CTN, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
		}
        
    // matching signs logics
    } else if (priortyCode == 50 || priortyCode == 49) {
        
        if (operatorToken->token == Lang::dicLang_and) { // Is LOGIC AND
            compile_LR_mathLogigBaseOperations(ByteCode::AND, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        } else { // Is LOGIC OR
            compile_LR_mathLogigBaseOperations(ByteCode::POR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken);
        }
        
    } else if (priortyCode == 40) { //equal sign OR pointer assignment

        //extract from 1 past the equal sign to the end of the tokens
		Tokens sub = tokens.extractInclusive(operatorIndex+1, tokens.getSize()-1, eraseCount, script);
        compiler(script, sub, debug, rCount);
		if (operatorToken->token == Lang::dicLang_equal) {
			script->addInstruction(Instruction(ByteCode::ASN, *leftToken));
		} else {
			script->addInstruction(Instruction(ByteCode::POI, leftToken->token));
		}
        tokens.extractInclusive(operatorIndex-1, operatorIndex+1, eraseCount, script);
        operatorIndex -= eraseCount;
            
    } else if (priortyCode == 0 || priortyCode == 1) {
		script->addInstruction(Instruction(ByteCode::PUSH, tokens.getToken(operatorIndex)));
		if (operatorIndex == tokens.getSize() - 1) {
			tokens.extractInclusive(operatorIndex, operatorIndex, eraseCount, script, true);
		} else {
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
			if (token->token == Lang::dicLang_comma) {
				return 10;
			}
			break;
		case 1:
			if (token->token != Lang::dicLang_equal && token->token != Lang::dicLang_pointer) {
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
/** Evaluate a declaration group of tokens:
 *  We need to see -> Barckets, BraceOpen after Barckets, No keywords, No braces. 
 */
int Parser::evaluateFunctionDeclaration(Tokens &sub) {
	int size = (int)sub.getSize();
	bool inArguBrackets = false;
	bool finishedArguBrackets = false;
	int nestedBrackets = 0;
	std::string brackOpen = Lang::dicLang_braketOpen;
	std::string brackClose = Lang::dicLang_braketClose;
	std::string bracesOpen = Lang::dicLang_bracesOpen;
	std::string bracesClose = Lang::dicLang_bracesClose;
	for (int i = 0; i < size; i++) {
		Token* token = sub.getTokenObject(i);
		if (i == 0 && token->token != brackOpen) {
			break;
		}
		if (!inArguBrackets && token->token == brackOpen) {
			inArguBrackets = true;
		} else if (inArguBrackets  && token->token == brackOpen) {
			nestedBrackets++;
		} else if (inArguBrackets  && token->token == brackClose && nestedBrackets > 0) {
			nestedBrackets--;
		} else if (inArguBrackets  && token->token == brackClose && nestedBrackets < 1) {
			inArguBrackets = false;
			finishedArguBrackets = true;
		} else if (!inArguBrackets && finishedArguBrackets && token->token == bracesOpen) {
			return 0;
		} else if (inArguBrackets) {
			if (token->token == bracesOpen || token->token == bracesClose) { return 14;  }
			if (token->type  == TokenType::KEYWORD) { return 15; }
		} else if (!inArguBrackets) {
			if (!finishedArguBrackets &&  (token->token == bracesOpen || token->token == bracesClose)) { return 16; }
			if (token->token == brackClose) { return 17; }
		}
	}
	return 13;
}
/** Evaluate a var token to make sure its a variable name and not something else.
 *
 */
bool Parser::evaluateVarNotObjectCall(Token* token) {
	if (token == nullptr) { return false; }
	if (token->type != TokenType::VAR) { return false; }
	std::string candid = token->token;
	int size = (int)candid.length();
	for (int i = 0; i < size; i++) {
		if (candid[i] == Lang::LangSubObject) { continue; }
		if (Lang::LangIsNamingAllowed(candid[i])) { continue; }
		return false;
	}
	return true;
}
/** Evaluates an array square braket to make sure its valid:
 *  will not allow keywords; braces and not structure commas.
 *  
 *  Raw strings are treated as length.
 *  Doubles are cast to integer.
 *
 */
int Parser::evaluateArraySbrackets(Tokens &sub) {
	int size = sub.getSize();
	Token* token = nullptr;
	for (int i = 0; i < size; i++) {
		token = sub.getTokenObject(i);
		if (token->token == Lang::dicLang_equal ||
			token->token == Lang::dicLang_pointer ||
			token->token == Lang::dicLang_bracesOpen ||
			token->token == Lang::dicLang_bracesClose
		) {
			return 1;
		}
		if (token->type == TokenType::KEYWORD) {
			return 2;
		}
	}
	return 0;
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
bool Parser::isDelimiter(const std::string& c) {
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
    return (c == Lang::LangGeneralSpace) ? true : false;
}
bool Parser::isSpace(const std::string& c) {
    return (c == Lang::dicLang_space) ? true : false;
}
/** Check whether we entering using a string:
 * 
 * @param char|string c
 * @return boolean
 */
bool Parser::isQstring(const char& c) {
    return c == Lang::LangStringIndicator;
}
bool Parser::isQstring(const std::string& c) {
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
bool Parser::isLetter(const std::string& c) {
    return Lang::LangIsNamingAllowed(c);
}
/** Indicate true of false if a specific character constant, c, is a digit or not
 * 
 * @param char|string c
 * @return boolean
 */
bool Parser::isDigit(const std::string& c) {
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
bool Parser::isKeyword(std::string s) {
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
	return getDelimiterPriorty(currentToken, currentTokenType);
}
/** Translates an delimiter string to a Precedence Priority
 * 
 * @param string toCheckToken
 * @param TokenType toCheckType
 * @return integer
 */
int Parser::getDelimiterPriorty(std::string toCheckToken, TokenType toCheckType) {
	if (toCheckType == TokenType::KEYWORD) {
		return 5000;
	}
	else if (toCheckToken == Lang::dicLang_bracesClose) {
		return 4999;
	}
	else if (toCheckToken == Lang::dicLang_braketOpen) {
		return 2000;
	}
	else if (toCheckToken == Lang::dicLang_sBraketOpen) {
		return 110;
	}
	else if (toCheckToken == Lang::dicLang_dec || toCheckToken == Lang::dicLang_inc) {
		return 91;
	}
	else if (toCheckToken == Lang::dicLang_power) {
		return 90;
	}
	else if (toCheckToken == Lang::dicLang_multi || toCheckToken == Lang::dicLang_divide) {
		return 80;
	}
	else if (toCheckToken == Lang::dicLang_plus || toCheckToken == Lang::dicLang_minus) {
		return 70;
	}
	else if (
		toCheckToken == Lang::dicLang_smaller ||
		toCheckToken == Lang::dicLang_greater ||
		toCheckToken == Lang::dicLang_greater_equal ||
		toCheckToken == Lang::dicLang_smaller_equal
	) {
		return 60;
	}
	else if (toCheckToken == Lang::dicLang_c_equal || toCheckToken == Lang::dicLang_c_nequal || toCheckToken == Lang::dicLang_c_tequal || toCheckToken == Lang::dicLang_c_ntequal) {
		return 59;
	}
	else if (toCheckToken == Lang::dicLang_and) {
		return 50;
	}
	else if (toCheckToken == Lang::dicLang_or) {
		return 49;
	}
	else if (toCheckToken == Lang::dicLang_equal || toCheckToken == Lang::dicLang_pointer) {
		return 40;
	}
	else if (toCheckToken == Lang::dicLang_comma) {
		return -100;
	}
	else {
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
    for (int i = 0; i < size; i++){
        if(tokens.getToken(i) == Lang::dicLang_comma){
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
	int nested = 0;
	for (int i = 0; i < size; i++) {
		std::string t = tokens.getToken(i);
		if (t == Lang::dicLang_braketOpen || t == Lang::dicLang_sBraketOpen) {
			nested++;
		}
		else if ((t == Lang::dicLang_braketClose || t == Lang::dicLang_sBraketClose)&& nested > 0) {
			nested--;
		}
		if (nested < 1 && t == Lang::dicLang_comma) {
			return true;
		}
	}
	return false;
}
/** Get floating commas count:
 *
 */
int Parser::countCommasNotNested(Tokens& sub) {
	int size = sub.getSize();
	if (size == 0) return 0;
	int count = 1;
	int nested = 0;
	for (int i = 0; i < size; i++) {
		std::string t = sub.getToken(i);
		if (t == Lang::dicLang_braketOpen || t == Lang::dicLang_sBraketOpen) {
			nested++;
		} else if ((t == Lang::dicLang_braketClose || t == Lang::dicLang_sBraketClose) && nested > 0) {
			nested--;
		} else if (nested < 1 && t == Lang::dicLang_comma) {
			count++;
		}
	}
	return count;
}
/** Get the comma index that is hanging and not nested in a group:
 *  -1 means no commas.
 */
int Parser::getCommaIndexNotNested(Tokens& tokens) {
	int size = tokens.getSize();
	int nested = 0;
	for (int i = 0; i < size; i++) {
		std::string t = tokens.getToken(i);
		if (t == Lang::dicLang_braketOpen || t == Lang::dicLang_sBraketOpen) {
			nested++;
		}
		else if ((t == Lang::dicLang_braketClose || t == Lang::dicLang_sBraketClose) && nested > 0) {
			nested--;
		}
		if (nested < 1 && t == Lang::dicLang_comma) {
			return i;
		}
	}
	return -1;
}
/** Parse any string to lower ASCII chars
 * 
 * @param string s
 * @return string
 */
std::string Parser::toLowerString(std::string *s) {
	std::string outBuffer = *s;
	std::transform(s->begin(), s->end(), outBuffer.begin(), ::tolower);
    return outBuffer;
}
/** Parse any string to lower ASCII chars
 * 
 * @param string s
 * @return string
 */
std::string Parser::toUpperString(std::string *s) {
	std::string outBuffer = *s;
    std::transform(s->begin(), s->end(), outBuffer.begin(), ::toupper);
    return outBuffer;
}

}