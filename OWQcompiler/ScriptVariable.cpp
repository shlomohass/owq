/* 
 * File:   ScriptVariable.h
 * Author: shlomo hassid
 *
 */

#include "ScriptVariable.h"

ScriptVariable::ScriptVariable() {
	value =			StackData();
    name            = ".invalid.null.initialize.";
    isRegistered    = false;
    address         = NULL;
	hasPointers		= 0;
	pointer			= nullptr;
    type            = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(std::string xName) {
	value = StackData();
	name = xName;
	isRegistered = false;
	address = NULL;
	hasPointers = 0;
	pointer = nullptr;
	type = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(std::string xName, StackData& sd) {
	value = sd;
	name = xName;
	isRegistered = false;
	address = NULL;
	hasPointers = 0;
	pointer = nullptr;
	type = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(std::string xName, ScriptVariable* sv) {
	value = StackData();
	name = xName;
	isRegistered = false;
	address = NULL;
	hasPointers = 0;
	pointer = sv;
	type = RegisteredVariable::GLOBAL_POINTER;
}
ScriptVariable::ScriptVariable(std::string xName, RegisteredVariable xType, void* xAddress) {
	value		 = StackData();
    name         = xName;
    isRegistered = true;
    address      = xAddress;
	hasPointers	 = false;
	pointer		 = nullptr;
    type         = xType;
}

std::string ScriptVariable::getName() {
	return name;
}

StackData ScriptVariable::getValue() {
	if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr) {
		return pointer->getValue();
	}
	return value;
}

StackData* ScriptVariable::getValuePointer() {
	if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr) {
		return pointer->getValuePointer();
	}
	return &value;
}

ScriptVariable* ScriptVariable::getPointer() {
	return pointer;
}

bool ScriptVariable::setValue(StackData& sd) {
	if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr) {
		return pointer->setValue(sd);
	}
	if (!isRegistered) {
		// Try to Mutate the variable otherwise Deep copy the StackData
		// We do this avoids a object destruction and new copy - this is good for perormance. 
		if (sd.isNumber()) {
			value.MutateToNumber(sd.getNumber());
		} else if (sd.isString()) {
			value.MutateToString(sd.getString());
		}
		else if (sd.isBoolean()) {
			value.MutateToBoolean(sd.getBoolean());
		} else if (sd.isNull()) {
			value.MutateToNull();
		} else {
			// Deep Copy
			value = sd;
		}
	} else {
		if (sd.isNumber() && type == RegisteredVariable::REGISTERED_DOUBLE) {
			double* v = static_cast<double*>(address);
			*v = sd.getNumber();
		} else if (sd.isString() && type == RegisteredVariable::REGISTERED_STRING) {
			std::string* v = static_cast<std::string*>(address);
			*v = sd.getString();
		} else if (sd.isBoolean() && type == RegisteredVariable::REGISTERED_DOUBLE) {
			bool* v = static_cast<bool*>(address);
			*v = sd.getRealBoolean();
		} else {
			return false;
		}
	}
	return true;
}

void ScriptVariable::deref() {
	type = RegisteredVariable::GLOBAL_FLEX;
	pointer = nullptr;
}

void ScriptVariable::setHasPointers() {
	hasPointers++;
}

void ScriptVariable::remHasPointers() {
	hasPointers--;
}

bool ScriptVariable::inPointerPath(const std::string& nameTocheck) {
	if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr) {
		if (name == nameTocheck) {
			return true;
		} 
		return pointer->inPointerPath(nameTocheck);
	}
	return false;
}

bool ScriptVariable::isRegister() {
	return isRegistered;
}
bool ScriptVariable::isPointed() {
	return hasPointers > 0;
}
std::string ScriptVariable::renderVariable() {
	if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr) {
		return pointer->renderVariable();
	}
	return "";
}

ScriptVariable::~ScriptVariable() {

}
