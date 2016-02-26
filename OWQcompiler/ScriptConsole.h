/* 
 * File:   ScriptConsole.h
 * Author: shlomo hassid
 */

#ifndef SCRIPTCONSOLE_H
#define	SCRIPTCONSOLE_H

#include "Setowq.h"
#include "StackData.h"
#include "ScriptVariable.h"
#include <iostream>
#include <sstream>
namespace Eowq
{
	class ScriptConsole {
	public:

		ScriptConsole();
		virtual ~ScriptConsole();

		//Generic methods:
		static void print(StackData* sd, bool debug);
		static StackData length(StackData* sd);
		static StackData type(StackData* sd);
		static StackData isNull(StackData* sd);
		static StackData isPointer(ScriptVariable* sv);
		static StackData isPointed(ScriptVariable* sv);
		static std::string toString(double number);
		
		//Generic methods only arrays:
		static StackData join(StackData* sd, StackData* dl);
		static StackData sum(StackData* sd);
		//Helpers 
		static std::string stackTypeName(SDtype sdtype);

		

	};
}
#endif	/* SCRIPTCONSOLE_H */

