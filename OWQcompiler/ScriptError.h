/* 
 * File:   ScriptError.h
 * Author: shlomo hassid
 *
 */

#ifndef SCRIPTERROR_H
#define	SCRIPTERROR_H

#include <string>
#include <iostream>
namespace Eowq
{
	class ScriptError {
	public:
		ScriptError();
		static void msg(std::string event);
		static void fatal(std::string event);
		static void warn(std::string event);
		static void render(std::string msg);
		virtual ~ScriptError();
	};
}
#endif	/* SCRIPTERROR_H */

