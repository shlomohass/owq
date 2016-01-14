/* 
 * File:   Script.h
 * Author: shlomo hassid
 *
 */

#ifndef SCRIPT_H
#define	SCRIPT_H


//#include <boost\filesystem.hpp>
//#include <boost\filesystem\fstream.hpp>
#include <boost\filesystem\operations.hpp>
#include <boost\filesystem\path.hpp>

enum ExecReturn {
	Ex_OK,
	Ex_RETURN,
	Ex_VAR_RESOLVE,
	Ex_NVAR_ASN,
	Ex_NULL_STACK_EXTRACTION
};

#include "Instruction.h"
#include "Method.h"
#include "Stack.h"
#include "ScriptConsole.h"
#include "Compute.h"
#include "Source.h"
#include <map>
#include <unordered_map>
#include <vector>

namespace fs = boost::filesystem;

class Parser;
class Script {
    
    friend class Parser;
	friend class Compute;
    friend class Tokens;

	// Debugger flag:
	bool script_debug;
	std::vector<Instruction> code;
	std::map<std::string, int> functionTable;
	std::unordered_map<std::string, ScriptVariable> variables;
    // As a function is executed more reference to it is pushed onto this variable
	std::vector<Method> functions;
	std::string currentExecutingMethod;
    int internalStaticPointer;
    
    void popActiveMethod();
    void pushMethod(int retAddress, std::string name);
    ExecReturn executeInstruction(Instruction &code, int& instructionPointer);
    ExecReturn executeInstruction(Instruction &code, int& instructionPointer, bool debug);
    int getFunctionAddress(std::string funcName);
    Method* getActiveMethod();

    int injectScript(Script* script);

    bool isSystemCall(std::string& object, std::string& functionName, Instruction& _xcode);
    bool validateExtension(std::wstring extension);
    int  mergeLinesAndCompile(Source* source, Parser* parser, int linenum, bool debug);
    ScriptVariable *getVariable(std::string varName);
    ScriptVariable *getGlobalVariable(std::string varName);
    
    void addInstruction(Instruction I);
    void addInstruction(Instruction I, bool allowRST);

public:
    Script();

    //Register variable to global scope from Application layer:
    bool registerVariable(std::string varName, RegisteredVariable type, void* address);
    //Register variable to global scope by execution runtime:
    bool registerVariable(std::string varName);
	bool registerVariable(std::string varName, StackData& sd);
    bool unregisterVariable(std::string varName);

    void execute(std::string funcCall);
    
	ExecReturn run();
	ExecReturn run(bool debug);
    
    bool loadFile(fs::wpath filename);
    bool loadFile(fs::wpath filename, bool debug);

    int getSize();
	void render();

    virtual ~Script();
};

#endif	/* SCRIPT_H */

