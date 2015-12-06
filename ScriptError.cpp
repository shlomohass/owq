/* 
 * File:   ScriptError.cpp
 * Author: shlomo hassid
 *
 */

#include "ScriptError.h"

ScriptError::ScriptError() {
    // TODO Auto-generated constructor stub

}

void ScriptError::msg(string event) {
    cout << "ScriptRuntimeError: " << event << endl;
}

void ScriptError::render(string msg) {
    cout << msg << endl;
}

ScriptError::~ScriptError() {
    // TODO Auto-generated destructor stub
}