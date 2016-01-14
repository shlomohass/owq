/* 
 * File:   ScriptError.cpp
 * Author: shlomo hassid
 *
 */

#include "ScriptError.h"

ScriptError::ScriptError() {

}

void ScriptError::msg(std::string event) {
	std::cout << "OWQ Compilation ERROR -> " << event << std::endl;
}

void ScriptError::fatal(std::string event) {
	std::cout << "OWQ Runtime ERROR -> " << event << std::endl;
}

void ScriptError::warn(std::string event) {
	std::cout << "OWQ Runtime WARNING -> " << event << std::endl;
}

void ScriptError::render(std::string msg) {
	std::cout << msg << std::endl;
}

ScriptError::~ScriptError() {

}