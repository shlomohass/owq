/* 
 * File:   Method.h
 * Author: shlomo hassid
 *
 */
#include "Script.h"
#include "Loop.h"

namespace Eowq {

	Loop::Loop() {
		varSize = 0;
		personalAddress = -1;
		var.reserve(5);
	}
	Loop::Loop(int address) {
		varSize = 0;
		personalAddress = address;
		var.reserve(5);
	}

	bool Loop::addVariable(std::string& name) {
		if (hasVariable(name)) {
			return false;
		}
		var[name] = ScriptVariable(name);
		varSize++;
		return true;
	}

	bool Loop::addVariable(std::string& name, StackData& sd) {
		if (hasVariable(name)) {
			return false;
		}
		var[name] = ScriptVariable(name, sd);
		varSize++;
		return true;
	}
	std::unordered_map<std::string, ScriptVariable>::iterator Loop::getVariableContainerEnd() {
		return var.end();
	}
	std::unordered_map<std::string, ScriptVariable>::iterator Loop::getVariableIt(std::string& name) {
		if (varSize == 0) return var.end();
		return var.find(name);
	}

	ScriptVariable* Loop::getVariable(std::string& name) {
		std::unordered_map<std::string, ScriptVariable>::iterator itV = getVariableIt(name);
		if (itV != var.end()) {
			return &itV->second;
		}
		return nullptr;
	}

	bool Loop::hasVariable(std::string& name) {
		if (varSize > 0 && var.count(name)) {
			return true;
		}
		return false;
	}

	void Loop::setPersonalAddress(int address) {
		personalAddress = address;
	}

	int  Loop::getPersonalAddress() {
		return personalAddress;
	}

	void Loop::derefInScope(std::string& name) {
		std::unordered_map<std::string, ScriptVariable>::iterator it;
		for (it = var.begin(); it != var.end(); it++) {
			ScriptVariable* found = it->second.getPointer();
			if (found != nullptr && found->getName() == name) {
				found->deref();
			}
		}
	}

	//Delete from scope:
	void Loop::deleteFromScope(Script* script, std::unordered_map<std::string, ScriptVariable>::iterator& it) {
		deleteFromScope(script, it, false);
	}
	void Loop::deleteFromScope(Script* script, std::unordered_map<std::string, ScriptVariable>::iterator& it, bool avoidDelete) {
		//Deref a pointer -> means go to actual variable and unmark pointed.
		if (it->second.getPointer() != nullptr) {
			it->second.getPointer()->remHasPointers();
		}
		//Deref if pointed -> means go to actual variable and null pointers.
		if (it->second.isPointed()) {
			script->derefBackwordsInScopes(it->second.getName());
		}
		//Kill arrays:
		if (it->second.getValue().isArray()) {
			//Releases any sub arrays:
			script->removeSubArrays(it->second.getValuePointer()->getArrayPointer());
			//Release the array
			script->arraySpace.erase(it->second.getValuePointer()->getArrayName());
		}
		if (!avoidDelete) {
			var.erase(it);
			varSize--;
		}
	}

	void Loop::resetScope(Script* script) {
		if (varSize) {
			//Backward deref:
			std::unordered_map<std::string, ScriptVariable>::iterator it;
			for (it = var.begin(); it != var.end(); it++) {
				//will only handle the cleaning without actual earasing!
				deleteFromScope(script, it, true);
			}
			//Clear safely:
			var.clear();
			varSize = 0;
		}
	}

	void Loop::renderScopeVars() {
		std::unordered_map<std::string, ScriptVariable>::iterator it;
		for (it = var.begin(); it != var.end(); it++) {
			std::cout << it->second.renderVariable() << std::endl;
		}
	}
}