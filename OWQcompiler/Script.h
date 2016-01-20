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

enum ScopeType {
	ST_METHOD,
	ST_LOOP
};

#include "Instruction.h"
#include "Loop.h"
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

struct OWQScope {
	ScopeType type;
	Method m;
	Loop   l;
	OWQScope() { }
	OWQScope(Method _m) { m = _m; type = ScopeType::ST_METHOD; }
	OWQScope(Loop _l) { l = _l; type = ScopeType::ST_LOOP; }
};

class Script {
    
    friend class Parser;
	friend class Loop;
	friend class Method;
	friend class Compute;
    friend class Tokens;

	// Debugger flag:
	bool script_debug;
	std::vector<Instruction> code;
	std::map<std::string, int> functionTable;
	std::unordered_map<std::string, ScriptVariable> variables;
    
	// Scope tracker:
	std::unordered_map<int, OWQScope> scopeStore;
	std::vector<OWQScope*> scope;
    
	int internalStaticPointer;
    
    void popActiveScope();
    void pushMethodScope(int address, int retAddress, std::string name);
	void pushLoopScope(int address);
	
	OWQScope* scopeStoreHas(int scopeAddress);

	int getFunctionAddress(std::string funcName);
	OWQScope* getActiveScope();
	OWQScope* getActiveScope(int scopeOffset);

    ExecReturn executeInstruction(Instruction &code, int& instructionPointer);
    ExecReturn executeInstruction(Instruction &code, int& instructionPointer, bool debug);


    int injectScript(Script* script);

    bool isSystemCall(std::string& object, std::string& functionName, Instruction& _xcode);
    bool validateExtension(std::wstring extension);
    int  mergeLinesAndCompile(Source* source, Parser* parser, int linenum, bool debug);

	std::unordered_map<std::string, ScriptVariable>::iterator getVariableIt(std::string& varName);
	std::unordered_map<std::string, ScriptVariable>::iterator getVariableIt(std::string& varName, int scopeOffset);
	std::unordered_map<std::string, ScriptVariable>::iterator getVariableIt(std::string& varName, int scopeOffset, bool& flag);
	std::unordered_map<std::string, ScriptVariable>::iterator getGlobalVariableIt(std::string& varName);

    ScriptVariable *getVariable(std::string& varName);
	ScriptVariable *getVariable(std::string& varName, int scopeOffset);
    ScriptVariable *getGlobalVariable(std::string& varName);
    
	void derefBackwordsInScopes(std::string& name);
	bool deleteInScopes(std::string& name);

    void addInstruction(Instruction I);
    void addInstruction(Instruction I, bool allowRST);

public:
    Script();

    //Register variable to global scope from Application layer:
    bool registerVariable(std::string& varName, RegisteredVariable type, void* address);
    //Register variable to global scope by execution runtime:
    bool registerVariable(std::string& varName); //intialize variable
	bool registerVariable(std::string& varName, StackData& sd); // variable
	int pointerVariable(std::string& varName, std::string& pointTo); // variable pointer
    bool unregisterVariable(std::string& varName);
	bool unregisterVariable(std::string& varName, bool notSys);
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

