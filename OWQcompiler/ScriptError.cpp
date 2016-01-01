/* 
 * File:   ScriptError.cpp
 * Author: shlomo hassid
 *
 */

#include "ScriptError.h"

ScriptError::ScriptError() {

}

void ScriptError::msg(string event) {
    cout << "Compilation ERROR: " << event << endl;
}

void ScriptError::render(string msg) {
    cout << msg << endl;
}

ScriptError::~ScriptError() {

}