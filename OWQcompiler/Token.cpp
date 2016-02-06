/* 
 * File:   Token.cpp
 * Author: shlomi
 * 
 * Created on 30 דצמבר 2015, 04:59
 */

#include "Token.h"

namespace Eowq {

	Token::Token(std::string _token, int _priority, TokenType _type, TokenFlag _flag) {
		token = _token;
		priority = _priority;
		type = _type;
		flag = _flag;
		rstPos = 0;
		arrayPush = false;
	}

	Token::Token(std::string _token, int _priority, TokenType _type, TokenFlag _flag, int _pos) {
		token = _token;
		priority = _priority;
		type = _type;
		flag = _flag;
		rstPos = _pos;
		arrayPush = false;
	}

	Token::~Token() {

	}

	void Token::setFlag(TokenFlag _flag) {
		flag = _flag;
	}

	void Token::setArrayTreatPush(bool toset) {
		arrayPush = toset;
	}

	void Token::setPriority(int _priority) {
		priority = _priority;
	}
}