/* 
 * File:   Script.h
 * Author: shlomo hassid
 *
 */

#ifndef SCRIPT_H
#define	SCRIPT_H

#include "Instruction.h"
#include "Method.h"
#include "Stack.h"
#include "ScriptConsole.h"
#include "Source.h"
#include <map>
#include <vector>

using namespace std;

class Parser;
class Script {
    
    friend class Parser;
    friend class Tokens;
    /**
     * The complete listing of instructions that define this script
     */
    vector<Instruction> code;
    /**
     * Created as instructions are added to the Script via addInstruction
     */
    map<string, int> functionTable;
    /**
     * only registered variables can be registered
     */
    map<string, ScriptVariable> variables;

    /**
     * As a function is executed more reference to it is pushed onto this variable
     */
    vector<Method> functions;
    /**
     * Used to as a tag to remind vm of the method that is being executing
     * Helpful when exiting a function, so we can clean up variables defined in that function
     */
    string currentExecutingMethod;
    
    int internalStaticPointer;
    
    void popActiveMethod();
    void pushMethod( int retAddress, string name);
    int	executeInstruction(Instruction code, int& instructionPointer);
    int	executeInstruction(Instruction code, int& instructionPointer, bool debug);
    int getFunctionAddress(string funcName);
    Method* getActiveMethod();

    int injectScript(Script* script);

    bool isSystemCall(string object, string functionName);
    bool validateExtension(string filename);
    int  mergeLinesAndCompile(Source* source, Parser* parser, int linenum, bool debug);
    ScriptVariable *getVariable(string varName);
    ScriptVariable *getGlobalVariable(string varName);
    
    void addInstruction(Instruction I);
    void addInstruction(Instruction I, bool allowRST);
public:
    Script();

    //Register variable to global scope from Application layer:
    bool registerVariable(string varName, RegisteredVariable type, void* address);
    //Register variable to global scope by execution runtime:
    bool registerVariable(string varName);
	bool registerVariable(string varName, StackData& sd);
    bool unregisterVariable(string varName);

    void render();

    void execute(string funcCall);
    
    void run();
    void run(bool debug);
    
    bool loadFile(string filename);
    bool loadFile(string filename, bool debug);

    int getSize();
    virtual ~Script();
};

#endif	/* SCRIPT_H */

