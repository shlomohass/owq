/* 
 * File:   Method.h
 * Author: shlomo hassid
 *
 */

#include "Method.h"

Method::Method() {
    var.reserve(4);
    name = "";
}
Method::Method(int xRetAddress, std::string setName) {
    var.reserve(4);
    retAddress = xRetAddress;
    name = setName;
}

Method::~Method() {
    var.clear();
}

bool Method::addVariable(std::string name, StackData& sd) {
	if (hasVariable(name)) {
		return false;
	}
    ScriptVariable sv = ScriptVariable(name,sd);
    var.push_back(sv);
	return true;
}

bool Method::addVariable(std::string name) {
	if (hasVariable(name)) {
		return false;
	}
    ScriptVariable sv = ScriptVariable(name);
    var.push_back(sv);
	return true;
}

ScriptVariable* Method::getVariable(std::string name) {
    for (int i=0; i<(int)var.size(); i++) {
        if ( var[i].getName() == name ) {
            return &var[i];
        }
    }
    return NULL;
}

bool Method::hasVariable(std::string name) {
	for (int i = 0; i<(int)var.size(); i++) {
		if (var[i].getName() == name) {
			return true;
		}
	}
	return false;
}

int Method::getReturnAddress() {
    return retAddress;
}

std::string Method::getName() {
    return name;
}

void Method::renderScopeVars() {
	std::cout << "--------------------------------------------" << std::endl;
	std::cout << "| OWQ Script -> Expose method scope        |" << std::endl;
	std::cout << "--------------------------------------------" << std::endl;
	std::cout << "METHOD -> " << name << std::endl;
    for (int i=0; i<(int)var.size(); i++) {
		std::cout << var[i].renderVariable() << std::endl;
    }
}