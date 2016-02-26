/**
 * Tokens.cpp
 * Author: shlomo hassid
 * 
 */



#include "Tokens.h"
#include "Lang.h"
#include "Script.h"

#include <sstream> 

namespace Eowq {


	Tokens::Tokens() {
		tokens.reserve(30);
		comparisonFlag = false;
		conditionFlag = false;
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
	void Tokens::addToken(std::string _token, int priortyCode, TokenType tokenType) {
		addToken(_token, priortyCode, tokenType, false);
	}
	void Tokens::addToken(std::string _token, int priortyCode, TokenType tokenType, bool useFlags) {
		Token token(_token, priortyCode, tokenType, TokenFlag::NORMAL);
		if (useFlags && tokenType == TokenType::DELIMITER) {
			if (Lang::LangIsComparison(_token)) {
				token.setFlag(TokenFlag::COMPARISON);
				comparisonFlag = true;
			}
			else if (Lang::LangIsOfCondition(_token)) {
				token.setFlag(TokenFlag::CONDITION);
				conditionFlag = true;
			}
		}
		tokens.push_back(token);
		if (getSize() > 1) {
			int size = getSize();
			//if previous token before this one is a variable
			if (isOpenParenthesis(size - 1) && isVar(size - 2)) {
				tokens[size - 2].priority = 2;
			}
		}
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
		if (startParenthesisIndex < 0 || startParenthesisIndex > getSize() - 1) {
			stdError("token extraction, startIndex out of bounds");
			return newTokenSet;
		}
		if (endParenthesisIndex < startParenthesisIndex || endParenthesisIndex > getSize() - 1) {
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
		for (i = 0; i < count + 2; i++) {
			//the contents extracted
			tokens.erase(tokens.begin() + startParenthesisIndex);
		}
		extractionCount = count + 2;
		if (setPointer && script->code.size() > 0) {
			script->internalStaticPointer += 1;
			Token rstToken(Lang::dicLangValue_rst_upper, 0, TokenType::RST, TokenFlag::NORMAL, script->internalStaticPointer);
			tokens.insert(tokens.begin() + startParenthesisIndex, rstToken);
			//Mark the last operation to set result with pointer:
			// script->code.back().setPointer(script->internalStaticPointer);
		} else {
			Token rstToken(Lang::dicLangValue_rst_upper, 0, TokenType::RST, TokenFlag::NORMAL, 0);
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
		return extractInclusive(startIndex, endIndex, extractionCount, script, false, true);
	}
	Tokens Tokens::extractInclusive(int startIndex, int endIndex, int& extractionCount, Script* script, bool setPointer) {
		return extractInclusive(startIndex, endIndex, extractionCount, script, setPointer, true);
	}
	Tokens Tokens::extractInclusive(int startIndex, int endIndex, int& extractionCount, Script* script, bool setPointer, bool setBackCode) {
		Tokens newTokenSet;
		if (startIndex < 0 || startIndex > getSize() - 1) {
			stdError("token extraction, startIndex out of bounds");
			return newTokenSet;
		}
		if (endIndex < startIndex || endIndex > getSize() - 1) {
			stdError("token extraction, endIndex out of bounds");
			return newTokenSet;
		}
		//copy from current token-set to new token-set
		int i;
		int count = 0; //the total count of values to extract
		for (i = startIndex; i < endIndex + 1; i++) {
			newTokenSet.addToken(tokens[i]);
			count++;
		}
		//erase
		for (i = 0; i < count; i++) { //erase the total number of tokens extracted
			//the contents extracted
			tokens.erase(tokens.begin() + startIndex);
		}
		//Set a RST:
		extractionCount = count;
		if (setPointer && script->code.size() > 0) {
			script->internalStaticPointer += 1;
			Token rstToken(Lang::dicLangValue_rst_upper, 0, TokenType::RST, TokenFlag::NORMAL, script->internalStaticPointer);
			tokens.insert(tokens.begin() + startIndex, rstToken);
			//Mark the last operation to set result with pointer:
			if (setBackCode)
				script->code.back().setPointer(script->internalStaticPointer);
		} else {
			Token rstToken(Lang::dicLangValue_rst_upper, 0, TokenType::RST, TokenFlag::NORMAL, 0);
			tokens.insert(tokens.begin() + startIndex, rstToken);
		}

		return newTokenSet;
	}
	Tokens Tokens::extractInclusiveWithoutRst(int startIndex, int endIndex, int& extractionCount) {
		Tokens newTokenSet;
		if (startIndex < 0 || startIndex > getSize() - 1) {
			stdError("token extraction, startIndex out of bounds");
			return newTokenSet;
		}
		if (endIndex < startIndex || endIndex > getSize() - 1) {
			stdError("token extraction, endIndex out of bounds");
			return newTokenSet;
		}
		//copy from current token-set to new token-set
		int i;
		int count = 0; //the total count of values to extract
		for (i = startIndex; i < endIndex + 1; i++) {
			newTokenSet.addToken(tokens[i]);
			count++;
		}
		//erase
		for (i = 0; i < count; i++) { //erase the total number of tokens extracted
			tokens.erase(tokens.begin() + startIndex);
		}
		extractionCount = count;
		return newTokenSet;
	}
	/** Extrack token set that are bound by a end of expression:
	 *
	 */
	Tokens Tokens::extractSubExpr() {
		Tokens newTokenSet;
		//copy from current token-set to new token-set
		int i;
		int count = 0; //the total count of values to extract
		for (i = 0; i < (int)tokens.size(); i++) {
			count++;
			if (tokens[i].type == TokenType::DELIMITER && tokens[i].token == Lang::dicLang_semicolon) {
				break;
			}
			newTokenSet.addToken(tokens[i]);
		}
		//erase
		for (i = 0; i < count; i++) {
			tokens.erase(tokens.begin());
		}
		return newTokenSet;
	}
	
	
	/** Remove token at index
	*
	* @param integer index
	*/
	void Tokens::pop(int index) {
		if (index < 0 || index > getSize() - 1) {
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
	bool Tokens::pushBefore(int index, std::string _token, int pri, TokenType type) {
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
	bool Tokens::pushAfter(int index, std::string _token, int pri, TokenType type) {
		Token token(_token, pri, type, TokenFlag::NORMAL);
		if (index + 1 >= (int)this->tokens.size()) {
			this->tokens.push_back(token);
		}
		else {
			auto pos_tokens = this->tokens.begin();
			advance(pos_tokens, index + 1);
			this->tokens.insert(pos_tokens, token);
		}
		return true;
	}
	/** Replace a specific token in set:
	*
	* @param int index
	* @param Token&
	*/
	void Tokens::replaceTokenAt(int index, Token& token) {
		Token* target = getTokenObject(index);
		if (target != nullptr)
			*target = token;
	}


	/* Check for not nested commas in token group:
	*
	*/
	bool Tokens::hasCommasNotNested() {
		int size = getSize();
		int nested = 0;
		for (int i = 0; i < size; i++) {
			Token* t = getTokenObject(i);
			if (t->token == Lang::dicLang_braketOpen || t->token == Lang::dicLang_sBraketOpen)
				nested++;
			else if ((t->token == Lang::dicLang_braketClose || t->token == Lang::dicLang_sBraketClose) && nested > 0)
				nested--;
			if (nested < 1 && t->token == Lang::dicLang_comma)
				return true;
		}
		return false;
	}
	/** Get first level commas (not nested) count:
	*
	*/
	int Tokens::countCommasNotNested() {
		int size = getSize();
		if (size == 0) return 0;
		int count = 1;
		int nested = 0;
		for (int i = 0; i < size; i++) {
			Token* t = getTokenObject(i);
			if (t->token == Lang::dicLang_braketOpen || t->token == Lang::dicLang_sBraketOpen)
				nested++;
			else if ((t->token == Lang::dicLang_braketClose || t->token == Lang::dicLang_sBraketClose) && nested > 0)
				nested--;
			else if (nested < 1 && t->token == Lang::dicLang_comma)
				count++;
		}
		return count;
	}
	/** Get the comma index that is hanging and not nested in a group:
	*  -1 means no commas.
	*/
	int Tokens::getCommaIndexNotNested() {
		int size = getSize();
		int nested = 0;
		for (int i = 0; i < size; i++) {
			Token* t = getTokenObject(i);
			if (t->token == Lang::dicLang_braketOpen || t->token == Lang::dicLang_sBraketOpen)
				nested++;
			else if ((t->token == Lang::dicLang_braketClose || t->token == Lang::dicLang_sBraketClose) && nested > 0)
				nested--;
			if (nested < 1 && t->token == Lang::dicLang_comma)
				return i;
		}
		return -1;
	}


	/** get tokens count
	*
	* @return int
	*
	*/
	int Tokens::getSize() {
		return (int)tokens.size();
	}
	/** Get a specific token at index
	*
	* @param int index
	* @return string
	*/
	std::string Tokens::getToken(int index) {
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
	/** Get a specific token at left position of index
	* will skip square brakets -> this is for arrays push delimiter
	* @param int fromIndex
	* @return Token*
	*/
	Token* Tokens::tokenLeftLookBeforeArrayTraverse(int fromIndex) {
		if (fromIndex < 1) { return nullptr; }
		bool inSquare = false;
		for (int i = fromIndex - 1; i >= 0; i--) {
			if (tokens[i].type == TokenType::DELIMITER && tokens[i].token == Lang::dicLang_sBraketClose) {
				inSquare = true;
				continue;
			}
			if (inSquare && tokens[i].type == TokenType::DELIMITER && tokens[i].token == Lang::dicLang_sBraketOpen) {
				inSquare = false;
				continue;
			}
			if (!inSquare) {
				return &tokens[i];
			}
		}
		return nullptr;
	}
	TokenFlag Tokens::getTokenFlag(int index) {
		if (index < 0 || index >= getSize()) {
			return TokenFlag::UNFLAG;
		}
		return tokens[index].flag;
	}
	/**
	 * Return the priority / weight associated with token at an index
	 * @param integer index
	 * @return integer
	 */
	int Tokens::getTokenPriorty(int index) {
		if (index < 0 || index > getSize() - 1) {
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
		for (int i = 0; i < size; i++) {
			if (tokens[i].priority > highest) {
				highest = tokens[i].priority;
				priortyCode = highest;
				index = i;
			}
		}
		return index;
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
		for (i = openIndex + 1; i < size; i++) {
			if (tokens[i].token == Lang::dicLang_braketOpen) {
				nested++;
			}
			else if (tokens[i].token == Lang::dicLang_braketClose && nested > 0) {
				nested--;
			}
			else if (tokens[i].token == Lang::dicLang_braketClose && nested == 0) {
				return i;
				break;
			}
		}
		return openIndex;
	}
	/** Find closing Square Brackets token of open index
	*
	* @param integer openIndex
	* @return  integer
	*/
	int Tokens::getMatchingCloseSquareBrackets(int openIndex) {
		int i;
		int nested = 0;
		int size = getSize();
		for (i = openIndex + 1; i < size; i++) {
			if (tokens[i].token == Lang::dicLang_sBraketOpen) {
				nested++;
			}
			else if (tokens[i].token == Lang::dicLang_sBraketClose && nested > 0) {
				nested--;
			}
			else if (tokens[i].token == Lang::dicLang_sBraketClose && nested == 0) {
				return i;
				break;
			}
		}
		return openIndex;
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


	/**
	* Returns true or false indicating if the index is a number
	* @param index of token, bound checking is implemented
	* @return true or false
	*/
	bool Tokens::isNumber(int index) {
		if (index < 0 || index > getSize() - 1) {
			return false;
		}
		if (tokens[index].type == TokenType::NUMBER) {
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
	/**
	* Returns true or false indicating if the index is a delimiter
	* @param index of token, bound checking is implemented
	* @return true or false
	*/
	bool Tokens::isDelimiter(int index) {
		if (index < 0 || index > getSize() - 1) {
			return false;
		}
		if (tokens[index].type == TokenType::DELIMITER) {
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
		if (index < 0 || index > getSize() - 1) {
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
		if (!isDelim) return false;
		if (tokens[index].token == Lang::dicLang_braketClose) {
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
		if (!isDelim) return false;
		if (tokens[index].token == Lang::dicLang_braketOpen) {
			return true;
		}
		return false;
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


	/** Expose token:
	*
	*/
	void Tokens::renderTokens() {
		std::cout << "   TOKENS        --> { ";
		for (int i = 0; i < getSize(); i++) {
			std::cout << "'" << tokens[i].token << "' ";
		}
		std::cout << "}" << std::endl;
	}
	void Tokens::renderTokensJoined() {
		std::cout << "   TOKENS-JOINED   --> ";
		for (int i = 0; i < getSize(); i++) {
			std::cout << tokens[i].token << " ";
		}
		std::cout << std::endl << std::endl;
	}
	void Tokens::renderTokenType() {
		std::string str;
		std::cout << "   TOKENS-TYPE   --> { ";
		for (int i = 0; i < getSize(); i++) {
			switch (tokens[i].type) {
			case TokenType::DELIMITER:str = "DLM"; break;
			case TokenType::NUMBER:	str = "NUM"; break;
			case TokenType::STRING:	str = "STR"; break;
			case TokenType::VAR:		str = "VAR"; break;
			case TokenType::KEYWORD:	str = "KEY"; break;
			case TokenType::NONE:		str = "NON"; break;
			case TokenType::RST:		str = "RST"; break;
			default:
				str = "UNKNOWN";
			}
			if (str == Lang::dicLangValue_rst_upper) {
				std::cout << "'" << str << ":" << tokens[i].rstPos << "' ";
			}
			else {
				std::cout << "'" << str << "' ";
			}
		}
		std::cout << "}" << std::endl;
	}
	void Tokens::renderTokenPriorty() {
		std::cout << "   TOKENS-PRIORITY --> ";
		for (int i = 0; i < getSize(); i++) {
			std::cout << "[" << tokens[i].priority << "] ";
		}
		std::cout << "}" << std::endl;
	}


	/** clear tokens
	*
	*/
	void Tokens::clear() {
		tokens.clear();
		comparisonFlag = false;
		conditionFlag = false;
	}
	/** Report error to user
	*
	* @param msg
	*/
	void Tokens::stdError(std::string msg) {
		std::cout << "error: " << msg << std::endl;
	}
	Tokens::~Tokens() {
		clear();
	}
}