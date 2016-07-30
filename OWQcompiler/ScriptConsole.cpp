/* 
 * File:   ScriptConsole.h
 * Author: shlomo hassid
 * 
 */

#include "ScriptConsole.h"

namespace Eowq {

	ScriptConsole::ScriptConsole() {

	}

	/**
	 * Write to the console
	 * @param StackData sd
	 */
	void ScriptConsole::print(StackData* sd, bool debug) {
		if (debug && OWQ_DEBUG_LEVEL > 0) {
			std::cout << std::endl << "CONSOLE-OUT >> " << sd->getAsString() << std::endl;
			return;
		}
		std::cout << sd->getAsString();
	}
	/** Get a variable length
	*
	* @param StackData* sd
	*/
	StackData ScriptConsole::length(StackData* sd) {
		if (sd->isOftype(SDtype::SD_STRING) || sd->isOftype(SDtype::SD_NUMBER))
			return StackData((int)sd->getAsString().length());
		if (sd->isOftype(SDtype::SD_ARRAY))
			return StackData((int)sd->getArrayPointer()->size());
		return StackData(0);
	}
	/** Get a variable native type
	*
	* @param StackData* sd
	*/
	StackData ScriptConsole::type(StackData* sd) {
		return StackData(stackTypeName(sd->getType()));
	}
	StackData ScriptConsole::isNull(StackData* sd) {
		return StackData(sd->getType() == SDtype::SD_NULL);
	}
	StackData ScriptConsole::isPointer(ScriptVariable* sv) {
		return StackData(sv->getPointer() != nullptr);
	}
	StackData ScriptConsole::isPointed(ScriptVariable* sv) {
		return StackData(sv->isPointed());
	}
	std::string ScriptConsole::toString(double number) {
		std::stringstream ss;
		ss << number;
		return ss.str();
	}

	/**
     * Write to the console
	 * @param StackData sd
	*/
	StackData ScriptConsole::join(StackData* sd, StackData* dl) {
		if (sd->getType() != SDtype::SD_ARRAY)
			return StackData();
		std::vector<StackData>* arrayTarget = sd->getArrayPointer();
		int size = (int)arrayTarget->size();
		std::stringstream ss;
		ss << "";
		for (int i = 0; i < size; i++) {
			StackData* sdChild = &arrayTarget->at(i);
			if (sdChild->isArray()) {
				ss << join(sdChild, dl).getAsString() << dl->getAsString();
			} else {
				ss << sdChild->getAsString() << dl->getAsString();
			}
		}
		std::string res = ss.str();
		int dls = (int)dl->getAsString().length();
		return StackData(res.substr(0, res.length() - dls));
	}
	/**
	 * Split a string or a number:
	 * return codes: 0 -> Ok, 1 -> Bad value, 2 -> bad argu, 
	*/
	int ScriptConsole::split(StackData* sd, StackData* sp, std::vector<StackData>* arrayTarget) {
		if (sd->getType() != SDtype::SD_NUMBER && sd->getType() != SDtype::SD_STRING)
			return 1;
		if (sp->getType() != SDtype::SD_NUMBER && sp->getType() != SDtype::SD_STRING)
			return 2;
		std::string delim = sp->getAsString();
		std::string input = sd->getAsString();
		int counter = 0;
		int limit = 0;
		std::stringstream value;
		value << "";
		// in case the argument is a number or empty string:
		if (sp->isNumber() || (sp->isString() && delim.length() == 0)) {
			limit = sp->isString() ? 1 : (int)sp->getAsNumber();
			for (unsigned int i = 0; i<input.length(); i++) {
				if (limit > 0 && counter == limit) {
					arrayTarget->push_back(StackData(value.str()));
					value.str("");
					counter = 0;
				}
				value << input[i];
				counter++;
			}
			if (value.tellp() > 0) {
				arrayTarget->push_back(StackData(value.str()));
			}
		// in case the argument is a string delimiter
		} else {
			for (unsigned int i = 0; i<input.length(); i++) {
				if (value.tellp() > delim.length() && value.str().substr((int)value.tellp() - delim.length()) == delim) {
					arrayTarget->push_back(StackData(value.str().substr(0, (int)value.tellp() - delim.length())));
					value.str("");
				}
				value << input[i];
			}
			if (value.tellp() > 0) {
				arrayTarget->push_back(StackData(value.str()));
			}
		}
		return 0;
	}
	StackData ScriptConsole::sum(StackData* sd) {
		if (sd->getType() != SDtype::SD_ARRAY)
			return StackData();

		std::vector<StackData>* arrayTarget = sd->getArrayPointer();
		int size = (int)arrayTarget->size();
		double result = 0;
		for (int i = 0; i < size; i++) {
			StackData* sdChild = &arrayTarget->at(i);
			if (sdChild->isArray()) {
				result += sum(sdChild).getAsNumber();
			}
			else {
				result += sdChild->getAsNumber();
			}
		}
		return StackData(result);
	}
	StackData ScriptConsole::highest(StackData* sd) {
		if (sd->getType() != SDtype::SD_ARRAY) {
			return StackData();
		}
		std::vector<StackData>* arrayTarget = sd->getArrayPointer();
		int size = (int)arrayTarget->size();
		double result = 0;
		double temp = 0;
		for (int i = 0; i < size; i++) {
			StackData* sdChild = &arrayTarget->at(i);
			if (sdChild->isArray()) {
				temp = highest(sdChild).getAsNumber();
			} else {
				temp = sdChild->getAsNumber();
			}
			if (i == 0 || temp > result) {
				result = temp;
			}
		}
		return StackData(result);
	}
	StackData ScriptConsole::lowest(StackData* sd) {
		if (sd->getType() != SDtype::SD_ARRAY) {
			return StackData();
		}
		std::vector<StackData>* arrayTarget = sd->getArrayPointer();
		int size = (int)arrayTarget->size();
		double result = 0;
		double temp = 0;
		for (int i = 0; i < size; i++) {
			StackData* sdChild = &arrayTarget->at(i);
			if (sdChild->isArray()) {
				temp = lowest(sdChild).getAsNumber();
			}
			else {
				temp = sdChild->getAsNumber();
			}
			if (i == 0 || temp < result) {
				result = temp;
			}
		}
		return StackData(result);
	}
	std::string ScriptConsole::stackTypeName(SDtype sdtype) {
		switch (sdtype) {
			case SDtype::SD_ARRAY:		return "ARRAY";			break;
			case SDtype::SD_BOOLEAN:	return "BOOLEAN";		break;
			case SDtype::SD_NULL:		return "NULL";			break;
			case SDtype::SD_NUMBER:		return "NUMBER";		break;
			case SDtype::SD_OBJ:		return "OBJECT";		break;
			case SDtype::SD_OBJpointer: return "OBJECT";		break;
			case SDtype::SD_RST:		return "INTERNAL_RST";	break;
			case SDtype::SD_STRING:		return "STRING";		break;
		}
		return "UNKNOWN";
	}

	ScriptConsole::~ScriptConsole() {
		// TODO Auto-generated destructor stub
	}

}