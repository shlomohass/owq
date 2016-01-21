/* 
 * File:   Script.h
 * Author: shlomo hassid
 *
 */
#include "Script.h"
#include "Parser.h"

#include <math.h>
#include <fstream>
#include <streambuf>

namespace fs = boost::filesystem;

Script::Script() {
    code.reserve(300);	//pre-allocate 300 instruction space for byte-codes
	scope.reserve(50);	//pre allocate 50 spaces for scopes
	scopeStore.reserve(50);
    internalStaticPointer = 0;
	script_debug = OWQ_DEBUG;
}

void Script::addInstruction(Instruction I) {
    addInstruction(I, false);
}
void Script::addInstruction(Instruction I, bool allowRST) {
    //reject instructions with RST operands
    if (!allowRST && *I.getOperand() == Lang::dicLangValue_rst_upper) {
        return;
    }
    code.push_back(I);
    if (I.getCode() ==  ByteCode::FUNC) {
        functionTable[*I.getOperand()] = getSize()-1;
    }
}

/**
 *
 * Returns the address/index of the function requested by funcName
 * @param funcName
 * @return
 */
int Script::getFunctionAddress(std::string funcName){
	std::map<std::string, int>::iterator it = functionTable.find(funcName);
    if (it != functionTable.end()) {
        return it->second;
    } else {
        return -1;
    }
}

/** A DEBUGGER Function
 *  Display the script byte code
 * 
 */
void Script::render() {
    if (OWQ_DEBUG_EXPOSE_FUNCTION_TABLE && OWQ_DEBUG_LEVEL > 1) {
        Lang::printHeader("Function Tables");
        for(std::map<std::string, int>::const_iterator it = functionTable.begin(); it != functionTable.end(); ++it) {
			std::cout << " Key: " << it->first << ", Value: " << it->second << std::endl;
        }
        Lang::printSepLine(2);
    }
    
    Lang::printHeader("Generated Byte Code");
    for (int i=0; i<(int)code.size(); i++) {
		std::cout << " " << i << "\t" << code[i].toString() ;
        if(code.size()-1 == i) {
			std::cout << " <---- Injection point" << std::endl;
        } else {
			std::cout << std::endl;
        }
    }
}

int Script::getSize() {
    return code.size();
}

void Script::execute(std::string funcCallExp) {
    // Parser used to compile the expressing inside of funcCallExp
    Parser p;
    // holds the compiled set of instructions to successfully call the function(s) within this->code
    Script callScript;
    // generate the set of instructions represented in funcCallExp
    p.compile(&callScript, funcCallExp);
    int ip = this->getSize(); //points to end of instruction(getSize()-1) + 1
                              //effectively ip is the size of the script
                              //so when callScript is injected it points this script
                              //ip points perfectly to the instruction of where to
                              //start executing to properly use the script
    int orip = ip; //the original ip, save a copy, used for removing the injected code
    int injectionSize = injectScript(&callScript); //number of instructions injected, used for injection removal

    //-----------------------------------------------------------
    //Run the code												|
    //	start at instruction address ip							|
    //	terminates when executeInstruction method returns 0		|
    //-----------------------------------------------------------
    for (; ip < getSize(); ip++) {
        if ( executeInstruction(code[ip],ip)  == 0 ) {
            break; //execution is done when executeInstruction returns 0-->secondary to RET opcode
        }
    }
    //-----------------------------------------------------------
    //erase/release the amount of injection that was made earlier		|
    //-----------------------------------------------------------
    for(int i=0; i<injectionSize; i++){
        code.erase(code.begin()+orip);
    }
    
    //Stack::render();
}

ExecReturn Script::run() {
    return run(false);
}
ExecReturn Script::run(bool debug) {
    if ( debug && OWQ_DEBUG_EXPOSE_EXECUTION_STEPS && OWQ_DEBUG_LEVEL > 1) {
        Lang::printHeader("Chunk Execution steps");
    }
	ExecReturn retCode;
	int size = getSize();
	for (int ip = 0; ip < size; ip++) {
		retCode = executeInstruction(code[ip], ip, debug);
        if (retCode != ExecReturn::Ex_OK) {
            break;
        }
    }
	return retCode;
}

/**
 *
 * executeInstruction is the workhorse of the script calls.
 *
 * It functions as the Virtual Machine/ CPU.  As such, it performs
 * all the calculations, operations needed of it
 * @param code
 * @param instructionPointer
 * @param boolean debug DEFAULT : FALSE
 * @return integer
 */
ExecReturn Script::executeInstruction(Instruction &xcode, int& instructionPointer) {
    return executeInstruction(xcode, instructionPointer, false);
}
ExecReturn Script::executeInstruction(Instruction &xcode, int& instructionPointer, bool debug) {
    //For debugging expose execution step:
    if ( debug && OWQ_DEBUG_EXPOSE_EXECUTION_STEPS && OWQ_DEBUG_LEVEL > 1) {
        if (OWQ_DEBUG_EXPOSE_EXECUTIOM_STACK_STATE && OWQ_DEBUG_LEVEL > 2) {
            Stack::render();
        }
		std::cout << std::endl << " * Executing(" << xcode.getCode() << ") " << xcode.toString() << std::endl;
    }
	//Call Compute functions:
    switch (xcode.getCode()) {
        case ByteCode::NOP:
			return ExecReturn::Ex_OK;
        case ByteCode::SWA:
			return Compute::execute_swap();
        case ByteCode::SHT:
			return Compute::execute_shift();
        case ByteCode::PUSH:
			return Compute::execute_push(xcode, this);
        case ByteCode::RET:
			return Compute::execute_function_return(xcode, this, instructionPointer);
        case ByteCode::ASN:
			return Compute::execute_variable_assignment(xcode, this);
		case ByteCode::POI:
			return Compute::execute_pointer_assignment(xcode, this, instructionPointer);
		case ByteCode::UNS:
			return Compute::execute_variable_unset(xcode, this);
		case ByteCode::INCL:
		case ByteCode::INCR:
		case ByteCode::DECL:
		case ByteCode::DECR:
			return Compute::execute_math_inc_dec(xcode, this);
		case ByteCode::GTR:
			return Compute::execute_math_gtr(xcode);
        case ByteCode::LSR:
			return Compute::execute_math_lsr(xcode);
        case ByteCode::CVE:
			return Compute::execute_math_cve(xcode);
		case ByteCode::CVN:
			return Compute::execute_math_cvn(xcode);
		case ByteCode::CTE:
			return Compute::execute_math_cte(xcode);
		case ByteCode::CTN:
			return Compute::execute_math_ctn(xcode);
        case ByteCode::LOOP:	
			return ExecReturn::Ex_OK; //just acts as a marker
        case ByteCode::DONE:	
			return Compute::execute_done_block(xcode, this, instructionPointer);
        case ByteCode::EIF:
			return ExecReturn::Ex_OK; //end of function
        case ByteCode::AND:
			return Compute::execute_math_and(xcode);
        case ByteCode::POR:
			return Compute::execute_math_por(xcode);
		case ByteCode::BRE:
			return Compute::execute_loop_break(xcode, this, instructionPointer);
		case ByteCode::BIF:
			return Compute::execute_cond_break(xcode, this, instructionPointer);
        case ByteCode::CMP:
			return Compute::execute_cond_cmp(xcode, this, instructionPointer);
        case ByteCode::ELE:
			return Compute::execute_cond_else(xcode, this, instructionPointer);
        case ByteCode::ADD:
			return Compute::execute_math_add(xcode);
        case ByteCode::SUB:
			return Compute::execute_math_subtract(xcode);
        case ByteCode::MULT:
			return Compute::execute_math_mul(xcode);
        case ByteCode::DIV:
			return Compute::execute_math_divide(xcode);
        case ByteCode::EXPON:
			return Compute::execute_math_expon(xcode);
		case ByteCode::FUNC:
			return Compute::execute_function_create(xcode, this, instructionPointer);
        case ByteCode::ARG:
			return Compute::execute_arguments_declaration(xcode, this);
		case ByteCode::ARGC:
			return Compute::execute_arguments_count_check(xcode, this);
        case ByteCode::DEF:
			return Compute::execute_variable_declaration(xcode, this);
        case ByteCode::CALL:
			return Compute::execute_function_call(xcode, this, instructionPointer);
		case ByteCode::DPUSH:
			Compute::flagPush = true;
    }
	return ExecReturn::Ex_OK;
}

/** Push/Create a method and push it onto the scope
 * @param retAddress
 * @param name
 */
void Script::pushMethodScope(int address, int retAddress, std::string name) {
	OWQScope* scopePtr = scopeStoreHas(address);
	if (scopePtr == nullptr) {
		scopeStore[address] = OWQScope(Method(address, retAddress, name));
		scopePtr = &scopeStore[address];
	}
	scope.push_back(scopePtr);
}

/** Push/Create a loop and push it onto the scope
*/
void Script::pushLoopScope(int address) {
	OWQScope* scopePtr = scopeStoreHas(address);
	if (scopePtr == nullptr) {
		scopeStore[address] = OWQScope(Loop(address));
		scopePtr = &scopeStore[address];
	}
	scope.push_back(scopePtr);
}

OWQScope* Script::scopeStoreHas(int scopeAddress) {
	std::unordered_map<int, OWQScope>::iterator itS = scopeStore.find(scopeAddress);
	if (itS == scopeStore.end()) {
		return nullptr;
	}
	return &itS->second;
}
/** Remove the actively executing script method of the calling stack/method stack
 *
 */
void Script::popActiveScope() {
    if (scope.size() == 0) {
        return;
    }
    scope.pop_back();
}
/** Assign a global variable scope from Application layer 
 *  This means that a pointer address will be passed to the global.
 *  @param string name
 *  @param RegisteredVariable type will be a enum value
 *  @param void* address
 *  @return boolean
 */
bool Script::registerVariable(std::string& name, RegisteredVariable type, void* address) {
	if (variables.find(name) == variables.end()) {
		variables[name] = ScriptVariable(name, type, address);
		return true;
	}
	return false;
}
bool Script::registerVariable(std::string& name) {
	OWQScope* s = getActiveScope(0);
	ScriptVariable *sv = nullptr;
	if (s != nullptr) { //if there is a valid and active scope
		if (s->type == ScopeType::ST_LOOP) {
			return s->l.addVariable(name);
		} else {
			return s->m.addVariable(name);
		}
	}
	if (variables.find(name) == variables.end()) {
		variables[name] = ScriptVariable(name);
		return true;
	}
	return false;
}
bool Script::registerVariable(std::string& name, StackData& sd) {
	OWQScope* s = getActiveScope(0);
	ScriptVariable *sv = nullptr;
	if (s != nullptr) { //if there is a valid and active scope
		if (s->type == ScopeType::ST_LOOP) {
			return s->l.addVariable(name, sd);
		} else {
			return s->m.addVariable(name, sd);
		}
	}
	if (variables.find(name) == variables.end()) {
		variables[name] = ScriptVariable(name, sd);
		return true;
	}
	return false;
}
int Script::pointerVariable(std::string& name, std::string& pointTo) {
	//Set a variable pointer:
	bool isFound = true;
	std::unordered_map<std::string, ScriptVariable>::iterator itT = getVariableIt(pointTo, 0, isFound);
	if (isFound) {
		//Check for infinite reference:
		if (pointTo == name || itT->second.inPointerPath(name)) {
			return 2;
		}
		std::unordered_map<std::string, ScriptVariable>::iterator itS = getVariableIt(name);
		//if its a pointer remove ispointed by -1;
		if (itS->second.getPointer() != nullptr) {
			itS->second.getPointer()->remHasPointers();
		}
		int cacheHasPointers = itS->second.getPointedCounter();
		itS->second = ScriptVariable(name, &itT->second);
		itS->second.setPointedCounter(cacheHasPointers);
		itT->second.setHasPointers();
		return 0;
	}
	return 1;
}
/** Unregister a variable only if it exists:
 * @param string name
 * @return boolean
*/
bool Script::unregisterVariable(std::string& name) {
	return unregisterVariable(name, false);
}
bool Script::unregisterVariable(std::string& name, bool notSys) {
	ScriptVariable* candid = getVariable(name);
	if (candid != nullptr) {
		if (notSys && candid->isRegister()) {
			return false;
		}
		// remove from what it points to:
		if (candid->getPointer() != nullptr) {
			candid->getPointer()->remHasPointers();
		}
		// deref backwards:
		if (candid->isPointed()) {
			derefBackwordsInScopes(name);
		}
		//Finaly delete.
		return deleteInScopes(name);
	}
	return false;
}

void Script::derefBackwordsInScopes(std::string& name) {
//Deref in nested scopes:
	for (int i = (int)scope.size() - 1; i >= 0; i--) {
		if (scope[i]->type == ScopeType::ST_LOOP) {
			scope[i]->l.derefInScope(name);
		}
		else {
			scope[i]->m.derefInScope(name);
		}
	}
	//Deref in global scope:
	std::unordered_map<std::string, ScriptVariable>::iterator it_r;
	for (it_r = variables.begin(); it_r != variables.end(); it_r++) {
		if (it_r->second.getPointer() != nullptr && it_r->second.getPointer()->getName() == name) {
			it_r->second.deref();
		}
	}
}
bool Script::deleteInScopes(std::string& name) {
	std::unordered_map<std::string, ScriptVariable>::iterator it;
	bool deleted = false;
	for (int i = (int)scope.size() - 1; i >= 0; i--) {
		if (scope[i]->type == ScopeType::ST_LOOP) {
			it = scope[i]->l.getVariableIt(name);
			if (it != scope[i]->l.getVariableContainerEnd()) {
				scope[i]->l.deleteFromScope(it);
				deleted = true;
				break;
			}
		}
		else {
			it = scope[i]->m.getVariableIt(name);
			if (it != scope[i]->m.getVariableContainerEnd()) {
				scope[i]->l.deleteFromScope(it);
				deleted = true;
				break;
			}
		}
	}
	if (!deleted) {
		it = getGlobalVariableIt(name);
		if (it != variables.end()) {
			variables.erase(it);
		}
		else {
			return false;
		}
	}
	return true;
}
/**
 *
 * Returns the method that is on top of the stack.
 * @return
 */
OWQScope* Script::getActiveScope() {
	return getActiveScope(0);
}
OWQScope* Script::getActiveScope(int scopeOffset) {
	int size = (int)scope.size() - 1;
    if (size < 0 || scopeOffset > size) {
        return nullptr;
    }
    return scope[size - scopeOffset];
}
/** Get variable scope iterator
*  For the variable name. and finaly in the Global scope
* @param varName
* @param scopeOffset
* @return
*/
std::unordered_map<std::string, ScriptVariable>::iterator Script::getVariableIt(std::string& varName) {
	return getVariableIt(varName, 0);
}
std::unordered_map<std::string, ScriptVariable>::iterator Script::getVariableIt(std::string& varName, int scopeOffset) {
	OWQScope* s = getActiveScope(scopeOffset);
	std::unordered_map<std::string, ScriptVariable>::iterator it;
	if (s != nullptr) { //if there is a valid and active method
		if (s->type == ScopeType::ST_LOOP) {
			it = s->l.getVariableIt(varName);
			if (it == s->l.getVariableContainerEnd()) {
				return getVariableIt(varName, scopeOffset + 1);
			}
		} else {
			it = s->m.getVariableIt(varName);
			if (it == s->m.getVariableContainerEnd()) {
				return getVariableIt(varName, scopeOffset + 1);
			}
		}
	} else {
		//Not in scope so search in global scope
		it = getGlobalVariableIt(varName);
	}
	return it;
}
std::unordered_map<std::string, ScriptVariable>::iterator Script::getVariableIt(std::string& varName, int scopeOffset, bool& flag) {
	OWQScope* s = getActiveScope(scopeOffset);
	std::unordered_map<std::string, ScriptVariable>::iterator it;
	if (s != nullptr) { //if there is a valid and active method
		if (s->type == ScopeType::ST_LOOP) {
			it = s->l.getVariableIt(varName);
			if (it == s->l.getVariableContainerEnd()) {
				return getVariableIt(varName, scopeOffset + 1, flag);
			}
		} else {
			it = s->m.getVariableIt(varName);
			if (it == s->m.getVariableContainerEnd()) {
				return getVariableIt(varName, scopeOffset + 1, flag);
			}
		}
	} else {
		//Not in scope so search in global scope
		it = getGlobalVariableIt(varName);
		if (it == variables.end()) {
			flag = false;
		}
	}
	return it;
}
/**
* Seraches variable is GLOBAL scope.
* @param varName
* @return
*/
std::unordered_map<std::string, ScriptVariable>::iterator Script::getGlobalVariableIt(std::string& varName) {
	//------- Search in global scope:
	std::unordered_map<std::string, ScriptVariable>::iterator it;
	return variables.find(varName);
}
/** Get variable is scope sensitive it will recursivly search the scoped que 
 *  For the variable name. and finaly in the Global scope
 * @param varName
 * @param scopeOffset
 * @return
 */
ScriptVariable* Script::getVariable(std::string& varName) {
	return getVariable(varName, 0);
}
ScriptVariable* Script::getVariable(std::string& varName, int scopeOffset) {
	OWQScope* s = getActiveScope(scopeOffset);
    ScriptVariable *sv = nullptr;
    if (s != nullptr) { //if there is a valid and active method
		if (s->type == ScopeType::ST_LOOP) {
			sv = s->l.getVariable(varName);
		} else {
			sv = s->m.getVariable(varName);
		}
		if (sv == nullptr) {
			return getVariable(varName, scopeOffset + 1);
		}
    } else {
        //Not in scope so search in global scope
        sv = getGlobalVariable(varName);
    }
    return sv;
}
/**
 * Seraches variable is GLOBAL scope.
 * @param varName
 * @return
 */
ScriptVariable* Script::getGlobalVariable(std::string& varName) {
    //------- Search in global scope:
	std::unordered_map<std::string, ScriptVariable>::iterator it;
    it = variables.find(varName);
    if (it != variables.end()) {
        return &it->second; //return the address of the ScriptVariable
    }
    return nullptr;
}

/**
 * Functions to place the entire code from script into calling object
 * @param script
 * @return return amount of code injected
 */
int Script::injectScript(Script* script) {
    for (int i=0; i<script->getSize(); i++) {
        code.push_back(script->code[i]);
    }
    return script->getSize();
}

bool Script::isSystemCall(std::string& object, std::string& functionName, Instruction& _xcode) {
    
    //Console print:
	int sysCall = Lang::LangFindSystemLib(functionName);
    if (sysCall == 1 || sysCall == 2) { // Print
        StackData* sd = Stack::pop(0);
		if (sd != nullptr) {
			ScriptConsole::print(sd, this->script_debug);
			Stack::eraseAt(sd->getOrigin());
			Stack::runGC();
		}
		if (Compute::flagPush) { Compute::flagPush = false;  }
        return true;
    }
    
    //Handle objects
    if (!object.empty()) {
        /**
         * Here, we find the variable denoted by 'object', then depending on the value of funcName depends
         * on the variables behavior
         */
        ScriptVariable* sv = getVariable(object);
        if (sv != nullptr) {
            if (sysCall == 3) { // Length
				if (!Compute::flagPush) {
					Stack::push(ScriptConsole::length(sv->getValuePointer()));
					if (_xcode.getPointer() > 0) {
						Stack::setTopPointer(_xcode.getPointer());
					}
				}
            }
			else if (sysCall == 4) { //type
				if (!Compute::flagPush) {
					Stack::push(ScriptConsole::type(sv->getValuePointer()));
					if (_xcode.getPointer() > 0) {
						Stack::setTopPointer(_xcode.getPointer());
					}
				}
			}
			else if (sysCall == 5) { //isNull
				if (!Compute::flagPush) {
					Stack::push(ScriptConsole::isNull(sv->getValuePointer()));
					if (_xcode.getPointer() > 0) {
						Stack::setTopPointer(_xcode.getPointer());
					}
				}
			}
			else if (sysCall == 6) { //isPointer
				if (!Compute::flagPush) {
					Stack::push(ScriptConsole::isPointer(sv));
					if (_xcode.getPointer() > 0) {
						Stack::setTopPointer(_xcode.getPointer());
					}
				}
			}
			else if (sysCall == 7) { //isPointed
				if (!Compute::flagPush) {
					Stack::push(ScriptConsole::isPointed(sv));
					if (_xcode.getPointer() > 0) {
						Stack::setTopPointer(_xcode.getPointer());
					}
				}
			}
            else if (sysCall == 8) { //substr
				if (!Compute::flagPush) {
					if (sv->getValuePointer()->isString()) {
						Stack::render();
						StackData* sb = Stack::pop();	//second argument first
						StackData* sa = Stack::pop();	//first argument
						if (sb == nullptr || sa == nullptr) {
							ScriptError::msg("WARNINIG -> substr expects 2 arguments");
							Stack::push(sv->getValuePointer()->getString());
						} else {
							int a = (int)sa->getNumber(true);
							int b = (int)sb->getNumber(true);
							int originSB = sb->getOrigin();
							int originSA = sa->getOrigin();
							Stack::push(sv->getValuePointer()->getString().substr(a, b));
							Stack::eraseAt(originSB);
							Stack::eraseAt(originSA);
							Stack::runGC();
						}
						if (_xcode.getPointer() > 0) {
							Stack::setTopPointer(_xcode.getPointer());
						}
					}
				}
			} else {
				return false;
			}
			if (Compute::flagPush) { Compute::flagPush = false; }
            return true;
        } else {
            ScriptError::msg("Unable to find object " + object + " for system call " + functionName);
			if (Compute::flagPush) { Compute::flagPush = false; }
            return true;
        }
    }
    return false;
}

/** Validate if an extension is supported or not 
 * Extension type is set in Lang
 * 
 * @param filename
 * @return boolean
 */
bool Script::validateExtension(std::wstring extension) {
	std::string to_str(extension.begin(), extension.end());
    if(find(Lang::extensionLib.begin(), Lang::extensionLib.end(), to_str) != Lang::extensionLib.end()) {
        return true;
    } else {
        ScriptError::msg("script extension \"" + to_str + "\" does not match OpenWebQuery scripts");
    }
    return false;
}

int  Script::mergeLinesAndCompile(Source *source, Parser *parser, int linenum, bool debug) {
    //Clean
    source->cleanLine();
    //Push to line stack:
    source->pushLine(linenum);
    //Debugger -> expose the render source of lines and CODE:
    if (debug && OWQ_DEBUG_LEVEL > 0 && OWQ_DEBUG_EXPOSE_COMPILER_PARSE) { source->renderSource(); }
    //Compile stuff (line of code):
    return  parser->compile(this, source->getLines(), debug);
}

std::string errors[] = {
    "", 
    "1 script object is null",
    "2 recursive call max out script contains error",
    "3 syntax error for function definition",
    "4 if - statement syntax error",
    "5 while - statement syntax error",
    "6 else - statement syntax error",
	"7 Definition - expected definition of valriable name",
	"8 Missuse of Braces",
	"9 Declaration of variables should be followed by an assignment delimiter or by end of statement",
	"10 Found two commas in variable declaration",
	"11 Declaraion expression cannot contain keywords",
	"12 break statement can't use keywords",
	"13 Function declaration is expecting argument brackets followed by brace open char.",
	"14 Braces are not allowed in a argument of functions - missing a bracket close?",
	"15 Keywords are not allowed in as function arguments - missing a bracket close?",
	"16 Braces should be use after function argument brackets.",
	"17 Unexpected bracket close - missing a bracket open char?",
	"18 Function declaratin is missing a function name.",
	"19 Unset - unset expression is not legal you should unset only variables."
	"20 Missuse of Increment / Decrement operator - should be attached to a variable name only."
};

/** Loads and precompiles a script:
 * 
 * @param string filename
 * @param boolen debug
 * @return 
 * 
 */
bool Script::loadFile(fs::wpath filename) {
    return loadFile(filename, this->script_debug);
}       
bool Script::loadFile(fs::wpath filename, bool debug) {
    
	//Set debugger flag:
	script_debug = debug;

	//Force a file extension:
    if (!validateExtension(filename.extension().wstring())) {
        return false;
    }

    //Open file:
	std::wifstream input;
	std::wstring wstring_filename = filename.wstring();
	std::string string_filename(wstring_filename.begin(), wstring_filename.end());
    wchar_t cbuffer;
    input.open(filename.wstring());
    if (!input) {
        ScriptError::msg("unable to open " + string_filename + " for loading");
        return false;
    }
    //Load pre-compiler AKA Source parser:
    Source source(
        Lang::LangStringIndicator, 
        Lang::LangStringEscape,
        Lang::LangBlockOpenChar,
        Lang::LangBlockCloseChar,
        Lang::LangOperationEnd,
        Lang::LangMacroIndicator,
        Lang::LangMacroSetChar,
		Lang::LangFunctionOpenArguChar,
		Lang::LangFunctionCloseArguChar,
		Lang::LangArgumentSpacer
    );
    //Load compiler AKA  Parser:
    Parser parser;
    
    //Basic flags and counters:
    int  ret = 0;        //Return code of compiler
    int  linenum = 1;    //Keep track of line that is parsed
	bool macroFlag = false;
    bool flag = false;   //Whether to try validate the line or merge several lines.

    //Expose debugger output of pre compiling:
    if (debug && OWQ_DEBUG_LEVEL > 0 && OWQ_DEBUG_EXPOSE_COMPILER_PARSE) { Lang::printHeader("Compiler script parse and tokenize"); }

    //Render lines:
    while (input.get(cbuffer)) {
        
        //Pre parse:
        if (cbuffer != L'\n') {
            flag = source.pushChar((char)cbuffer);
            if (!flag) { continue; }
        }
        
        //We have a line check if its enough for compiling:
        if (source.validateLine()) {
            //Clean merge and compile block of code:
            ret = mergeLinesAndCompile(&source, &parser, linenum, debug);
            if (debug && OWQ_DEBUG_LEVEL > 0 && OWQ_DEBUG_EXPOSE_COMPILER_PARSE) {
                Lang::printSepLine(2);
            }
            if (ret != 0) {
                ScriptError::msg("compile error at line(" + source.getLineNumbers() + ") \"" + source.getLines() + "\" : " + errors[ret]);
                return false;
            }
            //Clear vectors and allocated memory for new pre-parse
            source.clearLines();
            
        } else {
            source.cleanLine();
            source.pushLine(linenum);
        }
        if (cbuffer == L'\n') {
            linenum++;
        }
    }
    //Execute all is left:
    ret = mergeLinesAndCompile(&source, &parser, linenum, debug);
    if (ret != 0) {
        ScriptError::msg("compile error at line(" + source.getLineNumbers() + ") \"" + source.getLines() + "\" : " + errors[ret]);
        return false;
    }
    //Clear vectors and allocated memory for new pre-parse
    source.clearLines();
    
    //Finished All show macros used if debugger is requested:
    if (debug && OWQ_DEBUG_LEVEL > 0 && OWQ_DEBUG_EXPOSE_COMPILER_MACRO_USE) { 
        Lang::printHeader("Macros and usage records");
        source.renderMacros();
        Lang::printSepLine(2);
    }
    
    if (debug && OWQ_DEBUG_LEVEL > 0 && OWQ_DEBUG_EXPOSE_COMPILED_CODE) { 
        this->render();
    }
    return true;
}

/** Destructor
 * 
 */
Script::~Script() {

}