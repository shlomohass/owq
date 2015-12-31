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
    
    static void print(string msg);
    static void print(string msg, string msg2);
    static string toString(double number);
    virtual ~ScriptConsole();
    
};

#endif	/* SCRIPTCONSOLE_H */

