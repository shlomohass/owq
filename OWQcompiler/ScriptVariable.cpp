/* 
 * File:   ScriptVariable.h
 * Author: shlomo hassid
 *
 */

#include "ScriptVariable.h"

//svalue       = "NaN";
//dvalue = OWQ_NAN;
ScriptVariable::ScriptVariable() {
	value =			StackData();
    name            = ".invalid.null.initialize.";
    isRegistered    = false;
    address         = NULL;
    type            = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(string xName) {
	value = StackData();
	name = xName;
	isRegistered = false;
	address = NULL;
	type = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(string xName, StackData& sd) {
	value = sd;
	name = xName;
	isRegistered = false;
	address = NULL;
	type = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(string xName, RegisteredVariable xType, void* xAddress) {
	value		 = StackData();
    name         = xName;
    isRegistered = true;
    address      = xAddress;
    type         = xType;
}

string ScriptVariable::getName() {
	return name;
}

StackData ScriptVariable::getValue() {
	return value;
}

bool ScriptVariable::setValue(StackData& sd) {
	if (!isRegistered) {
		value = sd;
	} else {
		if (sd.isNumber() && type == RegisteredVariable::REGISTERED_DOUBLE) {
			double* v = static_cast<double*>(address);
			*v = sd.getNumber();
		}
		else if (sd.isString() && type == RegisteredVariable::REGISTERED_STRING) {
			string* v = static_cast<string*>(address);
			*v = sd.getString();
		}
		else if (sd.isBoolean() && type == RegisteredVariable::REGISTERED_DOUBLE) {
			bool* v = static_cast<bool*>(address);
			*v = sd.getRealBoolean();
		} else {
			return false;
		}
	}
	return true;
}

string ScriptVariable::renderVariable() {
	return "";
}

ScriptVariable::~ScriptVariable() {

}
