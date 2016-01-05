/* 
 * File:   ScriptConsole.h
 * Author: shlomo hassid
 */

#ifndef SCRIPTCONSOLE_H
#define	SCRIPTCONSOLE_H

#include "Setowq.h"
#include <iostream>
#include <sstream>

using namespace std;

class ScriptConsole {
public:
    
    ScriptConsole();
    
    static void print(string msg, bool debug);
    static void print(string msg, string msg2, bool debug);
    static string toString(double number);
    virtual ~ScriptConsole();
    
};

#endif	/* SCRIPTCONSOLE_H */

