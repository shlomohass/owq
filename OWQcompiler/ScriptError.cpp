/* 
 * File:   ScriptError.cpp
 * Author: shlomo hassid
 *
 */

#include "ScriptError.h"

ScriptError::ScriptError() {

}

void ScriptError::msg(std::string event) {
	std::cout << "Compilation ERROR: " << event << std::endl;
}

void ScriptError::render(std::string msg) {
	std::cout << msg << std::endl;
}

ScriptError::~ScriptError() {

}