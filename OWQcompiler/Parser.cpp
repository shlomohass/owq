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
	"4 if - statement syntax error expected brace after expression.",
	"5 while - statement syntax error expected brace after expression.",
	"6 else - statement syntax error expected brace after expression.",
	"7 Definition - expected definition of variable name",
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
	"22 Assignments operators and block delimiters are not allowedinside Array square brackets",
	"23 Pointer to array element is not allowed.",
	"24 Array element of type pointer is not allowed."
};

/** Construct compileobj
*
*/
Compileobj::Compileobj() {
	eraseCount    = 0;
	priortyCode   = 0;
	operatorToken = nullptr;
	leftToken     = nullptr;
	rightToken    = nullptr;
}

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

		ret = compilerNew(script, subexp, debug, 0);

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
	bool trimmedSpace = false;
    //NOTE: after call to getToken
    //	    the token just created will be stored in 'currentToken'
    getToken(trimmedSpace);

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
        if (currentTokenType == TokenType::NUMBER && tokens.getSize() > 0 && !trimmedSpace) {
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
        getToken(trimmedSpace);
    }
}
/** Return the next token contain in the expression passed to function <b>compile(Script, string</b>
 * 
 * @return
 */
std::string Parser::getToken(bool& trimmedSpace) {
	trimmedSpace = false;
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
		trimmedSpace = true;
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
    
    if (tokens.getSize() == 0) return 0;

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
			if (tokens.hasCommasNotNested()) {
				//Scan to Define all recursivly:
				int commaIndex = tokens.getCommaIndexNotNested();
				Tokens sub = tokens.extractInclusive(operatorIndex + 1, commaIndex - 1, eraseCount, script);
				if (sub.getSize() > 1) {
					//Evaluate:
					int t1 = evaluateDeclarationSub(sub, true);
					if (t1 > 0) { return t1; } //Invalid return error code!
											   //Compile sub expression:
					compiler(script, sub, debug, rCount);
				}
				tokens.pop(operatorIndex + 1); //Pop RST of variable expression
				tokens.pop(operatorIndex + 1); //Pop comma
				avoidEvaluation = true;
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
			if (tokens.hasCommasNotNested()) {
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

    
	
    //-----------------------------------------------------------
    // Handle tokens with commas in them
    //			i.e x, 32+y, z
    // will seperate them into subs and re-compile:
    //-----------------------------------------------------------
    if (tokens.hasCommasNotNested()) {
		int commaIndex = tokens.getCommaIndexNotNested();
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
	// Handle square brackets [
	//-----------------------------------------------------------
	
	if (operatorToken->token == Lang::dicLang_sBraketOpen) { 
		
		//  bracket [
		//  get the close of this bracket square
		int closeOfSquareBrackets = tokens.getMatchingCloseSquareBrackets(operatorIndex);

		//extract the content and replace with RST
		Tokens sub = tokens.extractContentOfParenthesis(operatorIndex, closeOfSquareBrackets, eraseCount, script);

		//If its the first extract then move on (avoids garbage):

		//Evaluate sub tokens of array call:
		int evSBres = evaluateArraySbrackets(sub);
		if (evSBres > 0) { return evSBres == 1 ? 22 : 21; }
		
		//Count comma cells:
		int arrayElementsCount = sub.countCommasNotNested();

		//This will indicate an array push flag the left token, remove rst and continue:
		Token* leftOverLook = tokens.tokenLeftLookBeforeArrayTraverse(operatorIndex);
		if (arrayElementsCount == 0 && leftOverLook != nullptr && leftOverLook->type == TokenType::VAR) {
			
			leftOverLook->setArrayTreatPush(true);
			tokens.pop(operatorIndex); //Pop RST 

		// This will indicate an array traverse callee:

		} else if (leftOverLook != nullptr && leftOverLook->type == TokenType::VAR) {

			//Remove Rst and cache the tokens for later use when variable is called
			leftOverLook->setArrayTraverse(arrayElementsCount);
			tokens.pop(operatorIndex); //Pop RST 
			cachedSubsPool.push_back(sub); // This will save the path tokens for later
			leftOverLook->setTokenSubCache(cachedSubsPool.size() - 1); //Save the cache index.

		} else {

			//Probably a definition
			//Parse elements:

			int ret = 0;
			if (arrayElementsCount > 0) {
				// compile sub:
				mark(ParseMark::ARRAYDEF);
				ret = compiler(script, sub, debug, rCount);
				unmark();
			}

			//Rst pointer sync:
			if (tokens.getTokenObject(operatorIndex)->rstPos < script->internalStaticPointer) {
				tokens.getTokenObject(operatorIndex)->rstPos = script->internalStaticPointer++;
			} else if (script->internalStaticPointer == 0) {
				tokens.getTokenObject(operatorIndex)->rstPos = ++script->internalStaticPointer;
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

        compile_LR_mathLogigBaseOperations(ByteCode::EXPON, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
    
    //multi and division 
    } else if (priortyCode == 80) {	
    
        if (operatorToken->token == Lang::dicLang_multi) { //Multiple
            compile_LR_mathLogigBaseOperations(ByteCode::MULT, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        } else { //Divide
            compile_LR_mathLogigBaseOperations(ByteCode::DIV, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        }
        
    //add and subtract
    } else if (priortyCode == 70) { 
    
        
    //greater lesser
    } else if (priortyCode == 60) { 
        
        if (operatorToken->token == Lang::dicLang_greater) { // Is greater than
            compile_LR_mathLogigBaseOperations(ByteCode::GTR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        } else if (operatorToken->token == Lang::dicLang_smaller) { // Is smaller than
            compile_LR_mathLogigBaseOperations(ByteCode::LSR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        } else if (operatorToken->token == Lang::dicLang_greater_equal) { // Is greater or equal to
			compile_LR_mathLogigBaseOperations(ByteCode::GTRE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
		} else if (operatorToken->token == Lang::dicLang_smaller_equal) { // Is smaller or equal to
			compile_LR_mathLogigBaseOperations(ByteCode::LSRE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
		}
        
    //c-equals ==
    } else if (priortyCode == 59) {
        
        if (operatorToken->token == Lang::dicLang_c_equal) { // Is value Equal to
            compile_LR_mathLogigBaseOperations(ByteCode::CVE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        } else if (operatorToken->token == Lang::dicLang_c_nequal){ // Is value not Equal to
			compile_LR_mathLogigBaseOperations(ByteCode::CVN, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        } else if (operatorToken->token == Lang::dicLang_c_tequal) { // Is type Equal to
			compile_LR_mathLogigBaseOperations(ByteCode::CTE, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
		} else if (operatorToken->token == Lang::dicLang_c_ntequal) { // Is type not Equal to
			compile_LR_mathLogigBaseOperations(ByteCode::CTN, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
		}
        
    // matching signs logics
    } else if (priortyCode == 50 || priortyCode == 49) {
        
        if (operatorToken->token == Lang::dicLang_and) { // Is LOGIC AND
            compile_LR_mathLogigBaseOperations(ByteCode::AND, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        } else { // Is LOGIC OR
            compile_LR_mathLogigBaseOperations(ByteCode::POR, script, &tokens, operatorIndex, priortyCode, eraseCount, leftToken, rightToken, debug, rCount);
        }
        
    } else if (priortyCode == 40) { //equal sign OR pointer assignment

        //extract from 1 past the equal sign to the end of the tokens 
		//This will push the needed assign:
		Tokens sub = tokens.extractInclusive(operatorIndex+1, tokens.getSize()-1, eraseCount, script);
        compiler(script, sub, debug, rCount);

		//Compile cached left before:
		if (leftToken->arrayTraverse != -1) {
			int ret = compiler(script, cachedSubsPool.at(leftToken->subTokenCache), debug, rCount);
			if (ret != 0) return ret;
		}

		if (operatorToken->token == Lang::dicLang_equal) {
			script->addInstruction(Instruction(ByteCode::ASN, *leftToken));
		} else if (operatorToken->token == Lang::dicLang_equalAdd) {
			script->addInstruction(Instruction(ByteCode::ASNA, *leftToken));
		} else if (operatorToken->token == Lang::dicLang_equalSub) {
			script->addInstruction(Instruction(ByteCode::ASNS, *leftToken));
		} else {
			script->addInstruction(Instruction(ByteCode::POI, leftToken->token));
		}
        tokens.extractInclusive(operatorIndex-1, operatorIndex+1, eraseCount, script);
        operatorIndex -= eraseCount;
            
    } else if (priortyCode == 0 || priortyCode == 1) {

		//Compile cached before:
		if (operatorToken->arrayTraverse != -1) {
			int ret = compiler(script, cachedSubsPool.at(operatorToken->subTokenCache), debug, rCount);
			if (ret != 0) return ret;
		}
		if (getMark() == ParseMark::ARRAYDEF) {
			script->addInstruction(Instruction(ByteCode::PUSH, *operatorToken, operatorToken->rstPos), true);
		} else {
			script->addInstruction(Instruction(ByteCode::PUSH, *operatorToken, operatorToken->rstPos));
		}

		//This will force a RST pointer especially for stuff like (a) or (4):
		if (operatorIndex == tokens.getSize() - 1 && getMark() != ParseMark::ARRAYDEF) {
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

int Parser::compilerNew(Script* script, Tokens& tokens, bool debug, int rCount) {\
	
	//Early exits:
	if (!script || script == nullptr) return 1;
	if (tokens.getSize() == 0) return 0;
	//Debuging:
	if (debug && OWQ_DEBUG_EXPOSE_COMPILER_PARSE && OWQ_DEBUG_LEVEL > 1) {
		tokens.renderTokens();
		if (OWQ_DEBUG_LEVEL > 2) { tokens.renderTokenType(); }
		if (OWQ_DEBUG_LEVEL > 3) { tokens.renderTokenPriorty(); }
		if (OWQ_DEBUG_LEVEL > 2) { Lang::printEmpLine(1); }
	}
	//rCount is the nesting recursive calls limit:
	rCount++;
	if (rCount > 50) return 2;
	

	//set compile object:
	Compileobj comobj;
	int operatorIndex = tokens.getHighestOperatorPriorityIndex(comobj.priortyCode);
	if (operatorIndex > 0)
		comobj.leftToken	= tokens.getTokenObject(operatorIndex - 1);
	if (operatorIndex < tokens.getSize() && tokens.getSize() > 1)
		comobj.rightToken	= tokens.getTokenObject(operatorIndex + 1);
	comobj.operatorToken	= tokens.getTokenObject(operatorIndex);
	comobj.operatorTokenStr = comobj.operatorToken->token;

	//If none:
	if (comobj.operatorToken->token == ".none.")
		Tokens::stdError("there is no operator found");

	//--------------------------------------------------------------------
	// Keyword has the highest priority so execute immediately
	// Key word statements should rerun compilation ret is not valid here.
	//--------------------------------------------------------------------
	if (comobj.operatorToken->type == TokenType::KEYWORD) {

		//Validate good marks -> those marks are not allowed:
		if (getMark() == ParseMark::BREAKEXP) {
			return 12;
		}

		// Variable Definition:
		if (comobj.operatorToken->token == Lang::dicLangKey_variable) {
			return compile_variable_definition(comobj, operatorIndex, script, tokens, debug, rCount);

		// Variable Destruction: -> only naming and objectes & arrays element allowed.
		} else if (comobj.operatorToken->token == Lang::dicLangKey_unset) {
			return compile_variable_destructor(comobj, operatorIndex, script, tokens, debug, rCount);
		
		// Condition Expression:
		} else if (comobj.operatorToken->token == Lang::dicLangKey_cond_if) {
			return compile_condition_if(comobj, operatorIndex, script, tokens, debug, rCount);

		// Conditions Else block:
		} else if (comobj.operatorToken->token == Lang::dicLangKey_cond_else) {
			return compile_condition_else(comobj, operatorIndex, script, tokens, debug, rCount);

		// While Loop:
		} else if (comobj.operatorToken->token == Lang::dicLangKey_loop_while) {
			return compile_loop_while(comobj, operatorIndex, script, tokens, debug, rCount);
		
		// Loop break:
		} else if (comobj.operatorToken->token == Lang::dicLangKey_loop_break) {
			return compile_loop_break(comobj, operatorIndex, script, tokens, debug, rCount);

		// Condition break:
		} else if (comobj.operatorToken->token == Lang::dicLangKey_cond_break) {
			return compile_condition_break(comobj, operatorIndex, script, tokens, debug, rCount);
		}
	}

	//-----------------------------------------------------------
	// Handle token set with commas in them in the first layer
	//-----------------------------------------------------------
	if (tokens.hasCommasNotNested()) {
		return compile_comma_set(comobj, operatorIndex, script, tokens, debug, rCount);
	}

	int ret = 0;

	//------------------------------------------------------------
	// un-mark the end of a function, if, while loop
	//------------------------------------------------------------
	if (comobj.operatorToken->token == Lang::dicLang_bracesClose) {
		ret = compile_end_block(comobj, operatorIndex, script, tokens, debug, rCount);
	}

	//-----------------------------------------------------------
	// Handle parenthetical groupings ( )
	//-----------------------------------------------------------
	if (comobj.operatorToken->token == Lang::dicLang_braketOpen) {
		ret = compile_parenthetical_gouping(comobj, operatorIndex, script, tokens, debug, rCount);
	}

	//-----------------------------------------------------------
	// Handle square brackets [ ]
	//-----------------------------------------------------------

	if (comobj.operatorToken->token == Lang::dicLang_sBraketOpen) {
		ret = compile_squareb_grouping(comobj, operatorIndex, script, tokens, debug, rCount);
	}

	//-----------------------------------------------------------
	// Handle Math and Push
	//-----------------------------------------------------------
	switch (comobj.priortyCode) {
		//PUSH
		case 0: case 1:
			ret = compile_push(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		// INC, DEC
		case 91:
			ret = compile_inc_dec_op(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		// EXPONENT:
		case 90:
			ret = compile_expon(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		// MULT and DIV
		case 80:
			ret = compile_mul_div(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		// ADD and SUBTRACT
		case 70:
			ret = compile_add_sub(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		// GTR LSR operations:
		case 60:
			ret = compile_grt_lsr(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		// EQUALITY operations
		case 59:
			ret = compile_equality_op(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		// LOGIC GATES:
		case 50: case 49:
			ret = compile_logic_gates(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
		case 40:
			ret = compile_equal(comobj, operatorIndex, script, tokens, debug, rCount);
			break;
	}

	//-----------------------------------------------------------
	// Validate return message and continue compilation
	//-----------------------------------------------------------
	if (ret != 0) return ret;
	return compilerNew(script, tokens, debug, rCount);
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
int Parser::compile_comma_set(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	int commaIndex = tokens.getCommaIndexNotNested();
	Tokens sub = tokens.extractInclusive(0, commaIndex, comobj.eraseCount, script);
	sub.pop(commaIndex);	//remove comma
	tokens.pop(0);		    //remove rst
	operatorIndex -= comobj.eraseCount;
	int ret = compilerNew(script, sub, debug, rCount);
	if (ret > 0) return ret;
	return compilerNew(script, tokens, debug, rCount);
}
int Parser::compile_end_block(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	ParseMark ret = unmark(); //un-mark the grouping maked earlier
	if (ret == ParseMark::WHILE) { //loop
		script->addInstruction(Instruction(ByteCode::DONE, Lang::dicLangKey_loop_while));
	} else if (ret == ParseMark::IF) {
		script->addInstruction(Instruction(ByteCode::DONE, Lang::dicLangKey_cond_if));
	} else if (ret == ParseMark::ELSE) {
		script->addInstruction(Instruction(ByteCode::DONE, Lang::dicLangKey_cond_else));
	} else {
		//Bad palcement of the barces
		return 8;
	}
	//Remove the brace:
	tokens.pop(operatorIndex);
	return 0;
}
int Parser::compile_LR_mathLogigBaseOperations(ByteCode bc, Script*& script, Tokens* tokens, int &operatorIndex, int &priority, int &eraseCount, Token* leftToken, Token* rightToken, bool debug, int rCount) {

	//Compile cached before of left:
	if (leftToken->arrayTraverse != -1) {
		int ret = compiler(script, cachedSubsPool.at(leftToken->subTokenCache), debug, rCount);
		if (ret != 0) return ret;
	}
    script->addInstruction(Instruction(ByteCode::PUSH, *leftToken, leftToken->rstPos), true);
	//Compile cached before of right:
	if (rightToken->arrayTraverse != -1) {
		int ret = compiler(script, cachedSubsPool.at(rightToken->subTokenCache), debug, rCount);
		if (ret != 0) return ret;
	}
	script->addInstruction(Instruction(ByteCode::PUSH, *rightToken, rightToken->rstPos), true);
    script->addInstruction(Instruction(bc));
    tokens->extractInclusive(operatorIndex - 1, operatorIndex + 1, eraseCount, script, true);
    operatorIndex -= eraseCount;
    return 0;
}
int Parser::compile_LR_math(ByteCode bc, Compileobj& comobj, int &operatorIndex, Script*& script, Tokens& tokens, bool debug, int rCount) {

	//Compile cached before of left:
	if (comobj.leftToken->arrayTraverse != -1) {
		int ret = compilerNew(script, cachedSubsPool.at(comobj.leftToken->subTokenCache), debug, rCount);
		if (ret != 0) return ret;
	}
	script->addInstruction(Instruction(ByteCode::PUSH, *comobj.leftToken, comobj.leftToken->rstPos), true);
	//Compile cached before of right:
	if (comobj.rightToken->arrayTraverse != -1) {
		int ret = compilerNew(script, cachedSubsPool.at(comobj.rightToken->subTokenCache), debug, rCount);
		if (ret != 0) return ret;
	}
	script->addInstruction(Instruction(ByteCode::PUSH, *comobj.rightToken, comobj.rightToken->rstPos), true);
	script->addInstruction(Instruction(bc));
	tokens.extractInclusive(operatorIndex - 1, operatorIndex + 1, comobj.eraseCount, script, true);
	operatorIndex -= comobj.eraseCount;
	return 0;
}
int Parser::compile_parenthetical_gouping(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	
	//get the close of this parenthesis
	int closeOfParenthesis = tokens.getMatchingCloseParenthesis(operatorIndex);
	
	//Validate close par:

	//extract the content and replace with RST later depending if its a group or a function
	Tokens sub = tokens.extractContentOfParenthesis(operatorIndex, closeOfParenthesis, comobj.eraseCount, script);
	operatorIndex -= 1;	//Just in case its a function call set next block to parse the call name,
	
						//Make appropriate function Call check:
	bool funcCall = false;
	if (tokens.getSize() > 1 && operatorIndex >= 0 && comobj.leftToken != nullptr) { //two or more																		 //if previous token before the parenthesis has a non zero priority of 2 then make function call
		if (tokens.getTokenPriorty(operatorIndex) && comobj.leftToken->type != TokenType::KEYWORD && comobj.leftToken->type != TokenType::DELIMITER) {
			funcCall = true;
			//Mark
			mark(ParseMark::GROUPDEFINE);
		}
	}

	int ret = compilerNew(script, sub, debug, rCount);

	//Make appropriate function Call
	if (funcCall) { //two or more
		if (operatorIndex == 0 && rCount == 1) { 
			// A garbage preventor: for no assignment functions:
			script->addInstruction(Instruction(ByteCode::DPUSH, "CALL"));
		}
		script->addInstruction(Instruction(ByteCode::CALL, comobj.leftToken->token, tokens.tokens[operatorIndex + 1].rstPos));
		tokens.pop(operatorIndex); //removes function name, operatorIndex points to function name
		unmark();
	}

	//Sync RST -> will pop RST and replace it with newest RST and sync latest Operation wil the same RST pos:
	tokens.extractInclusive(
		funcCall ? operatorIndex : operatorIndex + 1, 
		funcCall ? operatorIndex : operatorIndex + 1, 
		comobj.eraseCount, 
		script, 
		true
	);
	return ret;
}
int Parser::compile_squareb_grouping(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	//  get the close of this bracket square
	int closeOfSquareBrackets = tokens.getMatchingCloseSquareBrackets(operatorIndex);

	//extract the content and replace with RST
	Tokens sub = tokens.extractContentOfParenthesis(operatorIndex, closeOfSquareBrackets, comobj.eraseCount, script);

	//If its the first extract then move on (avoids garbage):

	//Evaluate sub tokens of array call:
	int evSBres = evaluateArraySbrackets(sub);
	if (evSBres > 0) { return evSBres == 1 ? 22 : 21; }
	//Count comma cells:
	int arrayElementsCount = sub.countCommasNotNested();
	//This will indicate an array push flag the left token, remove rst and continue:
	Token* leftOverLook = tokens.tokenLeftLookBeforeArrayTraverse(operatorIndex);
	if (arrayElementsCount == 0 && leftOverLook != nullptr && leftOverLook->type == TokenType::VAR) {
		leftOverLook->setArrayTreatPush(true);
		tokens.pop(operatorIndex); //Pop RST 
	}
	// This will indicate an array traverse callee:
	else if (leftOverLook != nullptr && leftOverLook->type == TokenType::VAR) {
		//Remove Rst and cache the tokens for later use when variable is called
		leftOverLook->setArrayTraverse(arrayElementsCount);
		tokens.pop(operatorIndex); //Pop RST 
		cachedSubsPool.push_back(sub); // This will save the path tokens for later
		leftOverLook->setTokenSubCache(cachedSubsPool.size() - 1); //Save the cache index.
	} else {
		//Probably a definition
		//Parse elements:
		int ret = 0;
		if (arrayElementsCount > 0) {
			// compile sub:
			mark(ParseMark::ARRAYDEF);
			ret = compilerNew(script, sub, debug, rCount);
			unmark();
		}

		//Rst pointer sync:
		tokens.extractInclusive(operatorIndex, operatorIndex, comobj.eraseCount, script, true, false);

		//If everything is fine add the constructor instructions:
		if (ret == 0) {
			std::stringstream strtoa;
			strtoa << arrayElementsCount;
			int test = tokens.getTokenObject(operatorIndex)->rstPos;
			script->addInstruction(Instruction(ByteCode::ARD, strtoa.str(), tokens.getTokenObject(operatorIndex)->rstPos), true);
		} else return ret;
	}
	// return compilerNew(script, tokens, debug, rCount);
	return 0;
}
int Parser::compile_variable_definition(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {

	//Set first variable on the right:
	if (comobj.rightToken != nullptr && comobj.rightToken->type == TokenType::VAR) {
		script->addInstruction(Instruction(ByteCode::DEF, *comobj.rightToken));
	} else return 7;

	//Check for several defines: note that define key word is still in the token set.
	bool avoidEvaluation = false;
	if (tokens.hasCommasNotNested()) {
		//Scan to Define all recursivly:
		int commaIndex = tokens.getCommaIndexNotNested();
		Tokens sub = tokens.extractInclusive(operatorIndex + 1, commaIndex - 1, comobj.eraseCount, script);
		if (sub.getSize() > 1) {
			//Evaluate:
			int t1 = evaluateDeclarationSub(sub, true);
			if (t1 > 0) return t1; //Invalid return error code!
			//Compile sub expression:
			compilerNew(script, sub, debug, rCount);
		}
		tokens.pop(operatorIndex + 1); //Pop RST of variable expression
		tokens.pop(operatorIndex + 1); //Pop comma
		avoidEvaluation = true;
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
		return compilerNew(script, tokens, debug, rCount);
	}
	//Finished:
	return 0;
}
int Parser::compile_variable_destructor(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	
	//Unset first variable on the right:
	if (comobj.rightToken != nullptr && comobj.rightToken->type == TokenType::VAR) {
		if (evaluateVarNotObjectCall(comobj.rightToken))
			script->addInstruction(Instruction(ByteCode::UNS, comobj.rightToken->token));
		else return 19;
	} else return 7;

	//This removes the define key word and the unset variable:
	tokens.pop(operatorIndex);
	tokens.pop(operatorIndex);
	//Scan to allow bulk unset:
	if (tokens.hasCommasNotNested()) {
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

int Parser::compile_condition_if(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	// if (expression) {
	tokens.pop(operatorIndex);	//erase the if keyword
	// (expression) {
	if (tokens.getToken(tokens.getSize() - 1) != Lang::dicLang_bracesOpen) 
		return 4;
	// Erase block open == brace open:
	tokens.pop(tokens.getSize() - 1);
	// (expression)
	mark(ParseMark::IF);
	// recursively evaluate the condition of this if statement
	int ret = compilerNew(script, tokens, debug, rCount);
	if (ret != 0) return ret;
	script->addInstruction(Instruction(ByteCode::CMP, comobj.operatorTokenStr));
	return 0;
}
int Parser::compile_condition_else(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	//else (expression) {
	tokens.pop(0);	//erase the else keyword
	// (expression) {
	if (tokens.getToken(tokens.getSize() - 1) != Lang::dicLang_bracesOpen)
		return 6;
	tokens.pop(tokens.getSize() - 1);
	// (expression)
	mark(ParseMark::ELSE); // 1- marks ELSE
	// recursively evaluate the condition of this else statement
	int ret = compilerNew(script, tokens, debug, rCount);
	if (ret != 0) return ret;
	script->addInstruction(Instruction(ByteCode::ELE, comobj.operatorTokenStr));
	return 0;
}

int Parser::compile_loop_while(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	// while (expression) {
	script->addInstruction(Instruction(ByteCode::LOOP, comobj.operatorTokenStr));
	tokens.pop(0);	//erase the while keyword
	// (expression) {
	if (tokens.getToken(tokens.getSize() - 1) != Lang::dicLang_bracesOpen)
		return 5;
	tokens.pop(tokens.getSize() - 1);
	// (expression)
	mark(ParseMark::WHILE);
	//recursively evaluate the condition of this while loop
	int ret = compilerNew(script, tokens, debug, rCount);
	if (ret != 0) return ret;
	script->addInstruction(Instruction(ByteCode::CMP, comobj.operatorTokenStr));
	return 0;
}
int Parser::compile_loop_break(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	//Check for number of breaks or default to one:
	if (comobj.rightToken == nullptr) {
		script->addInstruction(Instruction(ByteCode::PUSH, "1"));
		script->addInstruction(Instruction(ByteCode::BRE));
		return 0;
	}
	//mark break:
	mark(ParseMark::BREAKEXP);
	//Evaluate the break expression - keywords are not allowed:
	Tokens sub = tokens.extractInclusive(operatorIndex + 1, tokens.getSize() - 1, comobj.eraseCount, script);
	int ret = compilerNew(script, sub, debug, rCount); //compile the expression
	if (ret > 0) { return ret; }
	unmark(); //unmarks the break;
	script->addInstruction(Instruction(ByteCode::BRE));
	return 0;
}
int Parser::compile_condition_break(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	//Check for number of breaks:
	if (comobj.rightToken == nullptr) {
		script->addInstruction(Instruction(ByteCode::PUSH, "1"));
		script->addInstruction(Instruction(ByteCode::BIF));
		return 0;
	}
	//mark break same as loop break:
	mark(ParseMark::BREAKEXP);
	//Evaluate the break expression keywords are not allowed:
	Tokens sub = tokens.extractInclusive(operatorIndex + 1, tokens.getSize() - 1, comobj.eraseCount, script);
	int ret = compilerNew(script, sub, debug, rCount); //compile the expression
	if (ret > 0) { return ret; }
	unmark(); //unmarks the break;
	script->addInstruction(Instruction(ByteCode::BIF));
	return 0;
}


int Parser::compile_push(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	//Compile cached before:
	if (comobj.operatorToken->arrayTraverse != -1) {
		int ret = compilerNew(script, cachedSubsPool.at(comobj.operatorToken->subTokenCache), debug, rCount);
		if (ret != 0) return ret;
	}
	if (
			getMark() == ParseMark::ARRAYDEF
		||	getMark() == ParseMark::GROUPDEFINE
	)
		script->addInstruction(Instruction(ByteCode::PUSH, *comobj.operatorToken, comobj.operatorToken->rstPos), true);
	else
		script->addInstruction(Instruction(ByteCode::PUSH, *comobj.operatorToken, comobj.operatorToken->rstPos));
	tokens.extractInclusiveWithoutRst(operatorIndex, operatorIndex, comobj.eraseCount);
	operatorIndex -= comobj.eraseCount;
	return 0;
}
int Parser::compile_inc_dec_op(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	if (comobj.leftToken != nullptr && (comobj.leftToken->type == TokenType::VAR)) {
		//Is of type Postfix
		if (operatorIndex == 1 && rCount == 1) // A garbage preventor:
			script->addInstruction(Instruction(ByteCode::DPUSH, comobj.operatorToken->token));
		if (comobj.operatorToken->token == Lang::dicLang_dec) {
			script->addInstruction(Instruction(ByteCode::DECL, comobj.leftToken->token, comobj.leftToken->rstPos), true);
		} else {
			script->addInstruction(Instruction(ByteCode::INCL, comobj.leftToken->token, comobj.leftToken->rstPos), true);
		}
		tokens.extractInclusive(operatorIndex - 1, operatorIndex, comobj.eraseCount, script, true);
	} else if (comobj.rightToken != nullptr && (comobj.rightToken->type == TokenType::VAR)) {
		//Is of type Prefix
		if (operatorIndex == 0) // A garbage preventor:
			script->addInstruction(Instruction(ByteCode::DPUSH, comobj.operatorToken->token));
		if (comobj.operatorToken->token == Lang::dicLang_dec) {
			script->addInstruction(Instruction(ByteCode::DECR, comobj.rightToken->token, comobj.rightToken->rstPos), true);
		} else {
			script->addInstruction(Instruction(ByteCode::INCR, comobj.rightToken->token, comobj.rightToken->rstPos), true);
		}
		tokens.extractInclusive(operatorIndex, operatorIndex + 1, comobj.eraseCount, script, true);
	} else {
		//Error with increment decrement operator:
		return 20;
	}
	operatorIndex -= comobj.eraseCount;
	return 0;
}
int Parser::compile_expon(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	return compile_LR_math(ByteCode::EXPON, comobj, operatorIndex, script, tokens, debug, rCount);
}
int Parser::compile_add_sub(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	if (comobj.operatorToken->token == Lang::dicLang_plus) // ADD values 
		return compile_LR_math(ByteCode::ADD, comobj, operatorIndex, script, tokens, debug, rCount);
	// Subtract values
	return compile_LR_math(ByteCode::SUB, comobj, operatorIndex, script, tokens, debug, rCount);
}
int Parser::compile_mul_div(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	if (comobj.operatorToken->token == Lang::dicLang_multi) // Multiply values 
		return compile_LR_math(ByteCode::MULT, comobj, operatorIndex, script, tokens, debug, rCount);
	// Divide values
	return compile_LR_math(ByteCode::DIV, comobj, operatorIndex, script, tokens, debug, rCount);

}
int Parser::compile_grt_lsr(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	if (comobj.operatorToken->token == Lang::dicLang_greater) // Is greater than
		return compile_LR_math(ByteCode::GTR, comobj, operatorIndex, script, tokens, debug, rCount);
	if (comobj.operatorToken->token == Lang::dicLang_smaller) // Is smaller than
		return compile_LR_math(ByteCode::LSR, comobj, operatorIndex, script, tokens, debug, rCount);
	if (comobj.operatorToken->token == Lang::dicLang_greater_equal) // Is greater or equal to
		return compile_LR_math(ByteCode::GTRE, comobj, operatorIndex, script, tokens, debug, rCount);
	if (comobj.operatorToken->token == Lang::dicLang_smaller_equal) // Is smaller or equal to
		return compile_LR_math(ByteCode::LSRE, comobj, operatorIndex, script, tokens, debug, rCount);
	return 0;
}
int Parser::compile_equality_op(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	if (comobj.operatorToken->token == Lang::dicLang_c_equal) // Is value Equal to
		return compile_LR_math(ByteCode::CVE, comobj, operatorIndex, script, tokens, debug, rCount);
	if (comobj.operatorToken->token == Lang::dicLang_c_nequal) // Is value not Equal to
		return compile_LR_math(ByteCode::CVN, comobj, operatorIndex, script, tokens, debug, rCount);
	if (comobj.operatorToken->token == Lang::dicLang_c_tequal) // Is type Equal to
		return compile_LR_math(ByteCode::CTE, comobj, operatorIndex, script, tokens, debug, rCount);
	if (comobj.operatorToken->token == Lang::dicLang_c_ntequal) // Is type not Equal to
		return compile_LR_math(ByteCode::CTN, comobj, operatorIndex, script, tokens, debug, rCount);
	return 0;
}
int Parser::compile_logic_gates(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	if (comobj.operatorToken->token == Lang::dicLang_and) // Is LOGIC AND
		return compile_LR_math(ByteCode::AND, comobj, operatorIndex, script, tokens, debug, rCount);
	// Is LOGIC OR
	return compile_LR_math(ByteCode::POR, comobj, operatorIndex, script, tokens, debug, rCount);
}
int Parser::compile_equal(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount) {
	//extract from 1 past the equal sign to the end of the tokens 
	//This will push the needed assign:
	
	int ret;
	Tokens sub = tokens.extractInclusive(operatorIndex + 1, tokens.getSize() - 1, comobj.eraseCount, script);
	ret = compilerNew(script, sub, debug, rCount);
	if (ret != 0) return ret;

	// Validate left and right are present:

	//Compile cached left before:
	if (comobj.leftToken->arrayTraverse != -1) {
		ret = compilerNew(script, cachedSubsPool.at(comobj.leftToken->subTokenCache), debug, rCount);
		if (ret != 0) return ret;
	}

	if (comobj.operatorToken->token == Lang::dicLang_equal) {
		script->addInstruction(Instruction(ByteCode::ASN, *comobj.leftToken));
	}
	else if (comobj.operatorToken->token == Lang::dicLang_equalAdd) {
		script->addInstruction(Instruction(ByteCode::ASNA, *comobj.leftToken));
	}
	else if (comobj.operatorToken->token == Lang::dicLang_equalSub) {
		script->addInstruction(Instruction(ByteCode::ASNS, *comobj.leftToken));
	} else {
		//Validate pointer array restrictions:
		if (comobj.leftToken->arrayTraverse != -1 || comobj.leftToken->arrayPush)
			return 24;
		if (comobj.rightToken != nullptr && comobj.rightToken->arrayTraverse != -1)
			return 23;
		script->addInstruction(Instruction(ByteCode::POI, comobj.leftToken->token));
	}

	tokens.extractInclusive(operatorIndex - 1, operatorIndex + 1, comobj.eraseCount, script);
	operatorIndex -= comobj.eraseCount;

	return 0;
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
	} else if (toCheckToken == Lang::dicLang_bracesClose) {
		return 4999;
	} else if (toCheckToken == Lang::dicLang_braketOpen) {
		return 2000;
	} else if (toCheckToken == Lang::dicLang_sBraketOpen) {
		return 110;
	} else if (toCheckToken == Lang::dicLang_dec || toCheckToken == Lang::dicLang_inc) {
		return 91;
	} else if (toCheckToken == Lang::dicLang_power) {
		return 90;
	} else if (toCheckToken == Lang::dicLang_multi || toCheckToken == Lang::dicLang_divide) {
		return 80;
	} else if (toCheckToken == Lang::dicLang_plus || toCheckToken == Lang::dicLang_minus) {
		return 70;
	} else if (
		toCheckToken == Lang::dicLang_smaller ||
		toCheckToken == Lang::dicLang_greater ||
		toCheckToken == Lang::dicLang_greater_equal ||
		toCheckToken == Lang::dicLang_smaller_equal
		) {
		return 60;
	} else if (toCheckToken == Lang::dicLang_c_equal || toCheckToken == Lang::dicLang_c_nequal || toCheckToken == Lang::dicLang_c_tequal || toCheckToken == Lang::dicLang_c_ntequal) {
		return 59;
	} else if (toCheckToken == Lang::dicLang_and) {
		return 50;
	} else if (toCheckToken == Lang::dicLang_or) {
		return 49;
	} else if (
		toCheckToken == Lang::dicLang_equal ||
		toCheckToken == Lang::dicLang_equalAdd ||
		toCheckToken == Lang::dicLang_equalSub ||
		toCheckToken == Lang::dicLang_pointer
		) {
		return 40;
	} else if (toCheckToken == Lang::dicLang_comma) {
		return -100;
	} else {
		return 0;
	}
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