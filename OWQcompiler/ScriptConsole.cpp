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
    if (OWQ_DEBUG && OWQ_DEBUG_LEVEL > 0) {
        cout << endl << "CONSOLE-OUT >> " << msg1 << endl;
        return;
    }
    cout << msg1;
}
/**
 * Write to the console
 * @param msg
 */
void ScriptConsole::print(string msg1, string msg2) {
    if (OWQ_DEBUG && OWQ_DEBUG_LEVEL > 0) {
        cout << endl << "CONSOLE-OUT >> " << msg1 << ", " << msg2 << endl;
        return;
    }
    cout << "CONSOLE-OUT >> " << msg1 << ", " << msg2 << endl;
}

string ScriptConsole::toString(double number) {
    stringstream ss;
    ss << number;
    return ss.str();
}

ScriptConsole::~ScriptConsole() {
    // TODO Auto-generated destructor stub
}

