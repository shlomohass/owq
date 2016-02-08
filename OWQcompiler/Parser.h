/* 
 * File:   Parser.h
 * Author: shlomo hassid
 *
 */

#ifndef PARSER_H
#define	PARSER_H

#include "Setowq.h"
#include <iomanip>


#include "Lang.h"
#include "Script.h"
#include "Tokens.h"

namespace Eowq
{
	/**
	 *
	 * Parser functions as the workhorse of our script compilation
	 *
	 * It takes an expression and determines meaning
	 */
	enum ParseMark { UNMARK, FUNCTION, IF, WHILE, ELSE, GROUPDEFINE, BREAKEXP };

	class Parser {

		std::string expression;		//expression to evaluate and compile
		int expressionIndex;	//the index into the expression; i.e expression[expressionIndex]
		std::string currentToken;	//the currentToken as a function of expression
		std::vector<ParseMark> marks;		//helps to group related code branches: functions, if, while
		TokenType currentTokenType;	//describes the current token type
		std::vector<Tokens> cachedSubsPool;

		//---------------------------------------------------------
		// vital workers
		//---------------------------------------------------------
		void 	    tokenize(std::string& exp, Tokens& token);
		void 	    evaluateGroups(Tokens& token, TokenFlag flagToGroup);
		void 	    evaluateGroups(Tokens& token, TokenFlag flagToGroup, int startFrom);
		int   	    compiler(Script* script, Tokens& token, bool debug, int rCount);
		std::string getToken();
		void 	    mark(ParseMark markType);
		ParseMark   unmark();
		ParseMark   getMark();

		//----------------------------------------------------------
		// Evaluation functions at compilation time:
		//----------------------------------------------------------
		int    evaluateDeclarationSub(Tokens &sub, bool andTypes);
		bool   evaluateSetIncludeType(Tokens &sub, TokenType type);
		int    evaluateFunctionDeclaration(Tokens &sub);
		bool   evaluateVarNotObjectCall(Token* token);
		int    evaluateArraySbrackets(Tokens &sub);
		//----------------------------------------------------------
		// Aux- helper functions
		//----------------------------------------------------------
		bool whileNotDelimiter(int currentPos);
		bool isDelimiter(const std::string& c);
		bool isDelimiter(const char& c);
		bool isSpace(const char& c);
		bool isSpace(const std::string& c);
		bool isQstring(const char& c);
		bool isQstring(const std::string& c);
		bool isLetter(const char& c);
		bool isLetter(const std::string& c);
		bool isDigit(const char& c);
		bool isDigit(const std::string& c);
		bool isKeyword(std::string s);
		bool hasCommas(Tokens& token);
		bool hasCommasNotNested(Tokens& token);
		int  countCommasNotNested(Tokens& sub);
		int  getCommaIndexNotNested(Tokens& tokens);
		int  getDelimiterPriorty();
		int  getDelimiterPriorty(std::string toCheckToken, TokenType toCheckType);

		//----------------------------------------------------------
		// Compiler - Methods
		//----------------------------------------------------------
		bool compile_LR_mathLogigBaseOperations(ByteCode bc, Script*& script, Tokens* token, int &operatorIndex, int &priority, int &eraseCount, Token* leftToken, Token* rightToken);

	public:


		//Compilation errors messages:
		static std::string errors[];

		Parser();
		virtual ~Parser();

		//main work horses entry point compile -> tokenize -> compiler
		int compile(Script* script, std::string exp);
		int compile(Script* script, std::string exp, bool debug);

		//General:
		std::string toLowerString(std::string *s);
		std::string toUpperString(std::string *s);

	};
}
#endif	/* PARSER_H */

