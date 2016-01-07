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
ScriptVariable::ScriptVariable(std::string xName) {
	value = StackData();
	name = xName;
	isRegistered = false;
	address = NULL;
	type = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(std::string xName, StackData& sd) {
	value = sd;
	name = xName;
	isRegistered = false;
	address = NULL;
	type = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(std::string xName, RegisteredVariable xType, void* xAddress) {
	value		 = StackData();
    name         = xName;
    isRegistered = true;
    address      = xAddress;
    type         = xType;
}

std::string ScriptVariable::getName() {
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
			std::string* v = static_cast<std::string*>(address);
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

std::string ScriptVariable::renderVariable() {
	return "";
}

ScriptVariable::~ScriptVariable() {

}
