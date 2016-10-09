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
	enum ParseMark { 
		UNMARK, 
		FUNCTION, 
		IF, 
		WHILE, 
		ELSE, 
		GROUPDEFINE,
		BREAKEXP,
		ARRAYDEF
	};

	struct Compileobj {
		int eraseCount;
		int priortyCode;
		std::string operatorTokenStr;
		Token* operatorToken;
		Token* leftToken;
		Token* rightToken;
		Compileobj();
	};

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
		int   	    compilerNew(Script* script, Tokens& token, bool debug, int rCount);
		std::string getToken(bool& trimmedSpace);
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
		bool isKeyword(char& c);
		bool isObjectCall(char& c);

		int  getDelimiterPriorty();
		int  getDelimiterPriorty(std::string toCheckToken, TokenType toCheckType);

		//----------------------------------------------------------
		// Compiler - Methods
		//----------------------------------------------------------
		int compile_comma_set(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_end_block(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_LR_mathLogigBaseOperations(ByteCode bc, Script*& script, Tokens* token, int &operatorIndex, int &priority, int &eraseCount, Token* leftToken, Token* rightToken, bool debug, int rCount);
		int compile_LR_math(ByteCode bc, Compileobj& comobj, int &operatorIndex, Script*& script, Tokens& tokens, bool debug, int rCount);
		int compile_parenthetical_gouping(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_squareb_grouping(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_variable_definition(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_variable_destructor(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_condition_if(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_condition_else(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_loop_while(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_loop_break(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_condition_break(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);

		int compile_push(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_inc_dec_op(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_expon(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_add_sub(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_mul_div(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_grt_lsr(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_equality_op(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_logic_gates(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		int compile_equal(Compileobj& comobj, int operatorIndex, Script* script, Tokens& tokens, bool debug, int rCount);
		
	
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

