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
Method::Method(int xRetAddress, string setName) {
    var.reserve(4);
    retAddress = xRetAddress;
    name = setName;
}

Method::~Method() {
    var.clear();
}

bool Method::addVariable(string name, StackData& sd) {
	if (hasVariable(name)) {
		return false;
	}
    ScriptVariable sv = ScriptVariable(name,sd);
    var.push_back(sv);
	return true;
}

bool Method::addVariable(string name) {
	if (hasVariable(name)) {
		return false;
	}
    ScriptVariable sv = ScriptVariable(name);
    var.push_back(sv);
	return true;
}

ScriptVariable* Method::getVariable(string name) {
    for (int i=0; i<(int)var.size(); i++) {
        if ( var[i].getName() == name ) {
            return &var[i];
        }
    }
    return NULL;
}

bool Method::hasVariable(string name) {
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

string Method::getName() {
    return name;
}

void Method::renderScopeVars() {
    cout << "--------------------------------------------" << endl;
    cout << "| OWQ Script -> Expose method scope        |" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "METHOD -> " << name << endl;
    for (int i=0; i<(int)var.size(); i++) {
        cout << var[i].renderVariable() << endl;
    }
}