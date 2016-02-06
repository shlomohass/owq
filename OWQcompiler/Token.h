/* 
 * File:   Token.h
 * Author: shlomi
 *
 * Created on 30 דצמבר 2015, 04:59
 */

#ifndef TOKEN_H
#define	TOKEN_H

#include <iostream>
namespace Eowq
{
	enum TokenType { NONE, NUMBER, STRING, DELIMITER, VAR, KEYWORD, RST };
	enum TokenFlag { UNFLAG, NORMAL, COMPARISON, CONDITION };

	class Token {
	public:

		std::string token;
		int priority;
		TokenType type;
		TokenFlag flag;
		bool      arrayPush;
		int       rstPos;

		Token(std::string _token, int _priority, TokenType _type, TokenFlag _flag);
		Token(std::string _token, int _priority, TokenType _type, TokenFlag _flag, int _pos);
		virtual ~Token();

		void setFlag(TokenFlag _flag);
		void setArrayTreatPush(bool toset);
		void setPriority(int _priority);
	};
}
#endif	/* TOKEN_H */

