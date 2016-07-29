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
			if (!value.MutateTo(sd)) {
				value = sd;
			}
		} else {
			if (type == RegisteredVariable::REGISTERED_DOUBLE) {
				double* v = static_cast<double*>(address);
				*v = sd.getAsNumber();
			}
			else if (type == RegisteredVariable::REGISTERED_STRING) {
				std::string* v = static_cast<std::string*>(address);
				*v = sd.getAsString();
			}
			else if (sd.isBoolean() && type == RegisteredVariable::REGISTERED_BOOLEAN) {
				bool* v = static_cast<bool*>(address);
				*v = sd.getRealBoolean();
			} else {
				return false;
			}
		}
		return true;
	}

	int ScriptVariable::setValueAddSub(StackData& sd, bool sub) {
		if (type == RegisteredVariable::GLOBAL_POINTER && pointer != nullptr)
			return pointer->setValueAddSub(sd,sub);

		int preserveOrigin = sd.getOrigin();
		if (!isRegistered) {
			//First perform the operation:
			if (value.isString()) {
				if (!sub) {
					sd.MutateTo(StackData(value.getAsString() + sd.getAsString()));
				} else {
					return 25;
				}
			}
			else if (value.isNumber()) {
				if (!sub) sd.MutateTo(StackData(value.getAsNumber() + sd.getAsNumber()));
				else sd.MutateTo(StackData(value.getAsNumber() - sd.getAsNumber()));
			} else if (!sub && value.isArray() && sd.isArray()) {
				//We make a copy in case we use the same array:
				std::vector<StackData> tempVec = *sd.getArrayPointer();
				std::vector<StackData>* target = value.getArrayPointer();
				target->insert(target->end(), tempVec.begin(), tempVec.end());
				return 0;
			} else {
				return 24;
			}
			// Try to Mutate the variable otherwise Deep copy the StackData
			// We do this avoids a object destruction and new copy - this is good for perormance. 
			sd.setOrigin(preserveOrigin);
			if (!value.MutateTo(sd)) {
				value = sd;
			}
		} else {
			if (type == RegisteredVariable::REGISTERED_DOUBLE) {
				double* v = static_cast<double*>(address);
				if (sub) *v -= sd.getAsNumber();
				else  *v += sd.getAsNumber();
			}
			else if (type == RegisteredVariable::REGISTERED_STRING) {
				std::string* v = static_cast<std::string*>(address);
				if (!sub) *v += sd.getAsString();
				else return 25;
			} else {
				return 24;
			}
		}
		return 0;
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
			//Invert ele:
			int ele = path[index] >= 0 ? path[index] : (int)baseArr->size() + path[index];
			if (ele >= 0 && (int)baseArr->size() > ele) {
				candid = baseArr->at(ele).traverseInArray(path, index - 1);
			} else {
				return 8; //Not in array.
			}
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
			int ele = path[index] >= 0 ? path[index] : (int)baseArr->size() + path[index];
			if (ele >= 0 && (int)baseArr->size() > ele) {
				candid = baseArr->at(ele).traverseInArray(path, index - 1);
			} else {
				return nullptr; //Not in array.
			}
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