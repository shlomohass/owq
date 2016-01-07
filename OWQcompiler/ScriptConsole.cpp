/* 
 * File:   ScriptConsole.h
 * Author: shlomo hassid
 * 
 */

#include "ScriptConsole.h"

ScriptConsole::ScriptConsole() {
    // TODO Auto-generated constructor stub
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
	int l = 0;
	if (sd->isOftype(SDtype::SD_NUMBER) || sd->isOftype(SDtype::SD_STRING)) {
		l = (int)sd->getAsString().length();
		return StackData(l);
	}
	return StackData(0);
}
/** Get a variable native type
*
* @param StackData* sd
*/
StackData ScriptConsole::type(StackData* sd) {
	string littype = stackTypeName(sd->getType());
	return StackData(littype);
}

string ScriptConsole::toString(double number) {
    stringstream ss;
    ss << number;
    return ss.str();
}

string ScriptConsole::stackTypeName(SDtype sdtype) {
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

