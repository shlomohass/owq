/* 
 * File:   ScriptConsole.h
 * Author: shlomo hassid
 */

#ifndef SCRIPTCONSOLE_H
#define	SCRIPTCONSOLE_H

#include "Setowq.h"
#include "StackData.h"
#include <iostream>
#include <sstream>

using namespace std;

class ScriptConsole {
public:
    
    ScriptConsole();
    
	//Generic methods:
    static void print(StackData* sd, bool debug);
	static StackData length(StackData* sd);
	static StackData type(StackData* sd);
    static string toString(double number);

	//Helpers 
	static string stackTypeName(SDtype sdtype);

    virtual ~ScriptConsole();
    
};

#endif	/* SCRIPTCONSOLE_H */

