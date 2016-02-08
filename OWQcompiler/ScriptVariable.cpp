/* 
 * File:   ScriptVariable.h
 * Author: shlomo hassid
 *
 */

#include "ScriptVariable.h"

namespace Eowq {

	ScriptVariable::ScriptVariable() {
		value = StackData();
		name = ".invalid.null.initialize.";
		isRegistered = false;
		address = NULL;
		hasPointers = 0;
		pointer = nullptr;
		type = RegisteredVariable::GLOBAL_FLEX;
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
		value = StackData();
		name = xName;
		isRegistered = true;
		address = xAddress;
		hasPointers = 0;
		pointer = nullptr;
		type = xType;
	}

	std::string& ScriptVariable::getName() {
		return name;
	}

	StackData& ScriptVariable::getValue() {
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
			}
			else if (sd.isString()) {
				value.MutateToString(sd.getString());
			}
			else if (sd.isBoolean()) {
				value.MutateToBoolean(sd.getBoolean());
			}
			else if (sd.isNull()) {
				value.MutateToNull();
			}
			else {
				// Deep Copy
				value = sd;
			}
		}
		else {
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
			}
			else {
				return false;
			}
		}
		return true;
	}

	// 6-> is not array, 7 general error cant set, 8 path not correct
	int ScriptVariable::setValueInArray(StackData& sd, int* path, int index, bool push) {
		if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr)
			return pointer->setValueInArray(sd, path, index, push);
		if (type != RegisteredVariable::GLOBAL_FLEX || !value.isArray())
			return 6; //Not array.
		//Traverse
		std::vector<StackData>* baseArr = value.getArrayPointer();
		StackData* candid = &value;
		if (index > -1) {
			if ((int)baseArr->size() <= path[index]) {
				return 8; //Not array.
			}
			candid = baseArr->at(path[index]).traverseInArray(path, index - 1);
		}
		//Push to candid:
		if (push && candid != nullptr)
			return (candid->arrayPush(sd)) ? 0 : 7;
		//Set array element:
		if (!push && candid != nullptr)
			return (candid->MutateTo(sd)) ? 0 : 7;
		return 8;
	}
	
	StackData* ScriptVariable::getValueInArray(int* path, int index) {
		if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr)
			return pointer->getValueInArray(path, index);
		if (type != RegisteredVariable::GLOBAL_FLEX || !value.isArray())
			return nullptr; //Not array.
		//Traverse
		std::vector<StackData>* baseArr = value.getArrayPointer();
		StackData* candid = &value;
		if (index > -1) {
			if ((int)baseArr->size() <= path[index]) {
				return nullptr; //Not array.
			}
			candid = baseArr->at(path[index]).traverseInArray(path, index - 1);
		}
		return candid;
	}

	void ScriptVariable::deref() {
		type = RegisteredVariable::GLOBAL_FLEX;
		pointer = nullptr;
	}
	
	int ScriptVariable::getPointedCounter() {
		return hasPointers;
	}
	
	void ScriptVariable::setPointedCounter(int num) {
		hasPointers = num;
	}
	
	void ScriptVariable::setHasPointers() {
		hasPointers++;
	}

	void ScriptVariable::remHasPointers() {
		hasPointers--;
	}

	bool ScriptVariable::inPointerPath(const std::string& nameTocheck) {
		if (name[0] == nameTocheck[0] && name == nameTocheck) {
			return true;
		}
		if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr) {
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
}