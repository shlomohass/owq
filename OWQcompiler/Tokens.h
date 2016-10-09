/* 
 * File:   Tokens.h
 * Author: shlomo hassid
 *
 */

#ifndef TOKENS_H
#define	TOKENS_H

#include <iostream>
#include <vector>
#include "Token.h"

namespace Eowq
{

	/**
	 * Functions to hold script terms that characterize a line
	 */
	class Script;
	class Tokens {

		friend class Parser;

	private:
		std::vector<Token> tokens;
		//Flags which indicates if the set of tokens has special token which needs grouping
		bool comparisonFlag;
		bool conditionFlag;

	public:
		Tokens();

		void addToken(std::string _token, int priortyCode, TokenType tokenType, bool useFlags);
		void addToken(std::string _token, int priortyCode, TokenType tokenType);
		void addToken(Token _token);
		
		Tokens extractContentOfParenthesis(int startIndex, int endIndex, int& extractionCount, Script* script);
		Tokens extractContentOfParenthesis(int startIndex, int endIndex, int& extractionCount, Script* script, bool setPointer);
		Tokens extractInclusive(int startIndex, int endIndex, int& extractionCount, Script* script);
		Tokens extractInclusive(int startIndex, int endIndex, int& extractionCount, Script* script, bool setPointer);
		Tokens extractInclusive(int startIndex, int endIndex, int& extractionCount, Script* script, bool setPointer, bool setBackCode);
		Tokens extractInclusiveWithoutRst(int startIndex, int endIndex, int& extractionCount);
		Tokens extractSubExpr();

		void markAttachedTokenObj(int markId, int startIndex);

		void pop(int index);
		bool pushBefore(int index, std::string _token, int pri, TokenType type);
		bool pushAfter(int index, std::string _token, int pri, TokenType type);
		void replaceTokenAt(int index, Token& token);

		bool hasCommasNotNested();
		int  countCommasNotNested();
		int  getCommaIndexNotNested();

		int getSize();
		std::string getToken(int index);
		Token* getTokenObject(int index);
		Token* tokenLeftLookBeforeArrayTraverse(int fromIndex);
		TokenFlag getTokenFlag(int index);
		int  getTokenPriorty(int index);
		int getHighestOperatorPriorityIndex(int& priortyCod);
		int getMatchingCloseParenthesis(int openIndex);
		int getMatchingCloseSquareBrackets(int openIndex);

		bool setHasComparison();
		bool setHasCondition();

		bool isNumber(int index);
		bool isString(int index);
		bool isDelimiter(int index);
		bool isVar(int index);
		bool isCloseParenthesis(int index);
		bool isOpenParenthesis(int index);
		bool isKeyWord(int index);

		void renderTokens();
		void renderTokensJoined();
		void renderTokenType();
		void renderTokenPriorty();


		void clear();
		static void stdError(std::string msg);
		virtual ~Tokens();
	};
}
#endif	/* TOKENS_H */

