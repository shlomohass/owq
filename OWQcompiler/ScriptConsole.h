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

class ScriptConsole {
public:
    
    ScriptConsole();
    
	//Generic methods:
    static void print(StackData* sd, bool debug);
	static StackData length(StackData* sd);
	static StackData type(StackData* sd);
	static StackData isNull(StackData* sd);
	static StackData isPointer(ScriptVariable* sv);
	static StackData isPointed(ScriptVariable* sv);
    static std::string toString(double number);

	//Helpers 
	static std::string stackTypeName(SDtype sdtype);

    virtual ~ScriptConsole();
    
};

#endif	/* SCRIPTCONSOLE_H */

