/* 
 * File:   ScriptConsole.h
 * Author: shlomo hassid
 * 
 */

#include "ScriptConsole.h"

ScriptConsole::ScriptConsole() {
    // TODO Auto-generated constructor stub
}

/**
 * Write to the console
 * @param msg
 */
void ScriptConsole::print(string msg1) {
    cout << "CONSOLE-OUT >> " << msg1 << endl;
}
/**
 * Write to the console
 * @param msg
 */
void ScriptConsole::print(string msg1, string msg2) {
    cout << ">>" << msg1 << ", " << msg2 << endl;
}

string ScriptConsole::toString(double number) {
    stringstream ss;
    ss << number;
    return ss.str();
}

ScriptConsole::~ScriptConsole() {
    // TODO Auto-generated destructor stub
}

