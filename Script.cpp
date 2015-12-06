/* 
 * File:   Script.h
 * Author: shlomo hassid
 *
 */

#include "Script.h"
#include <math.h>
#include "Parser.h"
#include <fstream>

using std::ifstream;

Script::Script() {
    code.reserve(100);	//pre-allocate 100 instruction space for byte-codes
    functions.reserve(50);	//pre allocate 50 spaces for functions
}

void Script::addInstruction(Instruction I) {
    //reject instructions with RST operands
    if (I.getOperand() == "RST") {
        return;
    }
    code.push_back(I);
    if (I.getCode() ==  ByteCode::FUNC) {
        functionTable[I.getOperand()] = getSize()-1;
    }
}

/**
 *
 * Returns the address/index of the function requested by funcName
 * @param funcName
 * @return
 */
int Script::getFunctionAddress(string funcName){
    map<string, int>::iterator it = functionTable.find(funcName);
    if (it != functionTable.end()) {
        return it->second;
    } else {
        return -1;
    }
}

/**
 * Display the script byte code
 */
void Script::render() {
    cout << "--------------------------------------------" << endl;
    cout << "| OWQ Script -> func tables                |" << endl;
    cout << "--------------------------------------------" << endl;
    for(map<string, int>::const_iterator it = functionTable.begin(); it != functionTable.end(); ++it) {
        cout << " Key: " << it->first << ", Value: " << it->second << endl;
    }
    cout << endl;
    cout << "--------------------------------------------" << endl;
    cout << "| OWQ Script -> op byte code               |" << endl;
    cout << "--------------------------------------------" << endl;
    for (int i=0; i<(int)code.size(); i++) {
        cout << i << " "<< code[i].toString() ;
        if(code.size()-1 == i){
            cout << " <---- injection current point" <<endl;
        } else {
            cout << endl;
        }
    }
    cout << endl << endl;
}

int Script::getSize() {
    return code.size();
}

void Script::execute(string funcCallExp) {
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

void Script::run() {
    for (int ip = 0; ip < getSize(); ip++) {
        if ( executeInstruction(code[ip], ip)  == 0 ) {
            break; //execution is done when executeInstruction returns 0-->secondary to RET opcode
        }
    }
}

/**
 *
 * executeInstruction is the workhorse of the script calls.
 *
 * It functions as the Virtual Machine/ CPU.  As such, it performs
 * all the calculations, operations needed of it
 * @param code
 * @param instructionPointer
 * @return
 */
int Script::executeInstruction(Instruction xcode, int& instructionPointer) {
    //Uncomment for step by step execution
    Stack::render();
    cout << "Executing(" << xcode.getCode() << ") " << xcode.toString() << endl;
    //------------------------------------------
    //
    //------------------------------------------
    int ret = 1;	//1 = continue, 0 = terminate execution
    switch (xcode.getCode()) {
        case ByteCode::NOP:
            break;
        case ByteCode::PUSH:	
            //push always pushes immediate values onto the stack- if variable name appears, instead of pushing that variable
            //name, it pushes the variables **value***
            //this way there is no checking of if items pop off the stack is a variable or string, because all is assummmed
            //to be a value
            //cout << "search variable: " << endl;
            if (xcode.isOperandString()) {              //if operand is string
                //cout << "push is string: " << endl;
                if (xcode.operandHasQuote()) {          //if this operand is in the form---> ["what is this a string literal"]
                    //cout << "push string has q: " << endl;
                    //cout << "search variable: " << endl;
                    Stack::push(xcode.getOperand());	//save the string literal
                } else {
                    //cout << "push string no q: " << endl;
                    //the operand is in a form --> someWord <<---------no quotes around it, meaning it is a name of variable
                    //find that variable from the function/method stacking

                    ScriptVariable* sv = getVariable(xcode.getOperand());
                    
                    //cout << "variable: " << sv;
                    //complain if variable is not found
                    if (sv == NULL) {
                        ScriptError::msg("unable to resolve variable name: " + xcode.getOperand());
                    } else {
                        //else push the value
                        Stack::push(*sv);
                    }
                }
            } else {
                Stack::push(xcode.getNumber());
            }
            //push immediate value onto the stack
            //if operand is a reference to some variable, push its value onto the stack
            break;
        case ByteCode::RET:{
                Method *m = getActiveMethod();	//get active method
                if (m != NULL) {
                    instructionPointer = m->getReturnAddress() - 1 ;	//minus one because execution loop automatically increase by 1
                    //if functions stack size == 1, then we are at the end of the execution stack,
                    //meaning that we we can stop executing the script
                    if (functions.size() == 1) {
                        ret = 0;
                    }
                } else {
                    ScriptError::msg("instruction for return without active method");
                }
                //return pops the method/function off the method stack
                popActiveMethod();
            }
            break;
        case ByteCode::ASN:{
                StackData sd = Stack::pop();		//pop and get the value to assign
                ScriptVariable* sv = getVariable(xcode.getOperand());	//get the variable we wish to sign to
                if (sv == NULL) {
                    ScriptError::msg("unable to resolve symbol " + xcode.getOperand());
                } else {
                    //assign the value
                    sv->setFromStackData(sd);
                }
            }
            break;
        case ByteCode::GTR: {
                StackData b = Stack::pop();
                StackData a = Stack::pop();
                if (a.isNumber() && b.isNumber()) {                           //a = number AND b = number
                    Stack::push(a.getNumber() > b.getNumber());
                } else if (a.isNumber() && !b.isNumber()) {                  //a = number AND b = string
                    Stack::push(a.getNumber() > b.getString().length());
                } else if (!a.isNumber() && b.isNumber()) {                  //a = string AND b = number
                    Stack::push(a.getString().length() > b.getNumber());
                } else {                                                    //a = string AND b = string
                    Stack::push(a.getString().length() > b.getString().length());
                }
            }
        break;
        case ByteCode::LSR:{
                StackData b = Stack::pop();
                StackData a = Stack::pop();
                if(a.isNumber() && b.isNumber()){                                   //a = number AND b = number
                    Stack::push(a.getNumber() < b.getNumber());
                }else if(a.isNumber() && !b.isNumber()){                            //a = number AND b = string
                    Stack::push(a.getNumber() < b.getString().length());
                }else if(!a.isNumber() && b.isNumber()){                            //a = string AND b = number
                    Stack::push(a.getString().length() < b.getNumber());
                }else {                                                             //a = string AND b = string
                    Stack::push(a.getString().length() < b.getString().length());
                }
            }break;
        case ByteCode::LOOP:	
            //just acts as a marker
            break;
        case ByteCode::DONE:	
            //I call the instruction "DONE" a --repeater because it functions to change the instruction pointer to the first                    
            //imediate LOOP instruction by stepping backwards from the current code address
            //when at done, step backwards to first occurance of LOOP
            if(xcode.getOperand() == "while"){
                for(int j=instructionPointer; j > -1; j--){	//step backwards
                    if(code[j].getCode() == ByteCode::LOOP){			//if the instruction has an instruction code that matches LOOP
                        instructionPointer = j;				//set that instruction address to ip and break immediately
                        break;
                    }
                }
            } else if(xcode.getOperand() == "if") {
                    //do nothing
            } else {
                    //do nothing
            }
            break;
        case ByteCode::EIF:	//end of function
            break;
        case ByteCode::CMP:{	
                //I call CMP as the gate keeper because it functions to determine if the body of a condition can be executed or
                //not depending of the value on the stack
                StackData a = Stack::pop();	//used to overal condition of the while or if statement
                //1 - true : 0 - false
                if (!a.isNumber()) { 
                    ScriptError::msg("expected evaluation of boolean expression to be numeric"); 
                } else {
                    if (a.getNumber() == 1) { //condition is true
                        //continue
                    } else {
                        int matchCount=0;	//used to match respective cmp and done so we enter or exit the right conditon bodies
                        //-----------------------------------------------------------------------------------------------------|
                        //condition is false																				   |
                        //search for the next immediate "DONE" instruction, because it marks the end of the loop & escape	   |
                        //-----------------------------------------------------------------------------------------------------|
                        for (int j=instructionPointer; j<this->getSize(); j++) {
                            if (code[j].getCode() == ByteCode::CMP) {
                                ++matchCount;
                            }
                            if (code[j].getCode() == ByteCode::DONE) {
                                --matchCount;
                            }
                            if (matchCount == 0) {
                                    instructionPointer = j;	//change instruction pointer to be at this instancce of done instruction
                                    break;	//stop
                            } //end match case
                        }//end for loop
                    }//end else
                }//end else not number
            }
            break;
        case ByteCode::ADD:{
                StackData b = Stack::pop();
                StackData a = Stack::pop();
                if (a.isNumber() && b.isNumber()) {                         //a = number AND b = number
                    Stack::push(a.getNumber() + b.getNumber());
                } else if(a.isNumber() && !b.isNumber()) {                  //a = number AND b = string
                    Stack::push(a.numberValueToString() + b.getString());
                } else if(!a.isNumber() && b.isNumber()) {                  //a = string AND b = number
                    Stack::push(a.getString() + b.numberValueToString());
                } else {                                                    //a = string AND b = string
                        Stack::push(a.getString() + b.getString());
                }
            }
            break;
        case ByteCode::SUB: {
                StackData b = Stack::pop();
                StackData a = Stack::pop();
                if (a.isNumber() && b.isNumber()) {                     //a = number AND b = number
                    Stack::push(a.getNumber() - b.getNumber());
                } else if (a.isNumber() && !b.isNumber()) {             //a = number AND b = string
                        //Stack::push(a.numberValueToString() - b.getString());
                } else if (!a.isNumber() && b.isNumber()) {             //a = string AND b = number
                        //Stack::push(a.getString() - b.numberValueToString());
                } else {                                                //a = string AND b = string
                        //Stack::push(a.getString()  - b.getString());
                }
            }
            break;
        case ByteCode::MULT:{
                StackData b = Stack::pop();
                StackData a = Stack::pop();
                if (a.isNumber() && b.isNumber()) {                     //a = number AND b = number
                    Stack::push(a.getNumber() * b.getNumber());
                } else if(a.isNumber() && !b.isNumber()) {              //a = number AND b = string
                    //Stack::push(a.numberValueToString() - b.getString());
                } else if(!a.isNumber() && b.isNumber()) {              //a = string AND b = number
                    //Stack::push(a.getString() - b.numberValueToString());
                } else {                                                //a = string AND b = string
                    //Stack::push(a.getString()  - b.getString());
                }
            }
            break;
        case ByteCode::DIV:{
                StackData b = Stack::pop();
                StackData a = Stack::pop();
                if (a.isNumber() && b.isNumber()) {                     //a = number AND b = number
                    if (b.getNumber() == 0.0) {
                        ScriptError::msg("division by zero prevented, calculation aborted");
                        break;
                    }
                    Stack::push(a.getNumber() / b.getNumber());
                } else if (a.isNumber() && !b.isNumber()) {             //a = number AND b = string
                        //Stack::push(a.numberValueToString() - b.getString());
                } else if (!a.isNumber() && b.isNumber()) {             //a = string AND b = number
                        //Stack::push(a.getString() - b.numberValueToString());
                } else {                                                //a = string AND b = string
                        //Stack::push(a.getString()  - b.getString());
                }
            }break;
        case ByteCode::EXPON:{
                StackData b = Stack::pop();
                StackData a = Stack::pop();
                if(a.isNumber() && b.isNumber()){                       //a = number AND b = number
                    Stack::push( pow(a.getNumber() , b.getNumber()));
                }else if(a.isNumber() && !b.isNumber()){                //a = number AND b = string
                    //Stack::push(a.numberValueToString() - b.getString());
                }else if(!a.isNumber() && b.isNumber()){                //a = string AND b = number
                    //Stack::push(a.getString() - b.numberValueToString());
                }else {                                                 //a = string AND b = string
                    //Stack::push(a.getString()  - b.getString());
                }
            }break;
        case ByteCode::FUNC:
                //save the return address inside the newly created executing function
                pushMethod(instructionPointer + 1, xcode.getOperand());
            break;
        case ByteCode::ARG: {
                Method *m = getActiveMethod();
                if ( m == NULL ) {
                    ScriptError::msg("argument definition requires execution of method");
                } else {
                    StackData sd = Stack::pop();
                    if (sd.isNumber()) {
                        m->addVariable(xcode.getOperand(),sd.getNumber());
                    } else {
                        m->addVariable(xcode.getOperand(), sd.getString());
                    }
                }
            } break;
        case ByteCode::ARGC:
                if(xcode.getNumber() <= Stack::size()){

                } else {
                    ScriptError::msg("Current active method requires sufficient argument");
                }
            break;
        case ByteCode::DEF:{
                Method *m = getActiveMethod();
                if (m == NULL) {
                    //Assign in global scope:
                    registerVariable(xcode.getOperand());
                } else {
                    //Assign in method scope:
                    m->addVariable(xcode.getOperand());
                }
            }break;
        case ByteCode::CALL:{ //begin of call switch
                string operand = xcode.getOperand();
                string object = "NULL";
                string method = operand;
                int dotOperator = operand.find_first_of(".");

                if(dotOperator != -1){
                        object = operand.substr(0,dotOperator);
                        method = operand.substr(dotOperator+1);
                }
                if (isSystemCall(object, method)) {
                    break;
                } else {
                    int address = getFunctionAddress(operand);
                    if (address == -1) {
                        ScriptError::msg("unable to find method " + operand);
                    } else {
                        instructionPointer = address -1;
                    }
                    break;
                }
            }//end of call switch
    }
    return ret;
}

/**
 *
 * Push/Create a method and push it onto the stack
 *
 * This scheme helps to determine when to quit executing the script as a whole and return
 * control over to the main/non-scripted application.  Additionally, as pushMethod is call
 * the return address to which when this method is done executing will return to is passed
 *
 * @param retAddress
 */
void Script::pushMethod(int retAddress, string name) {
    functions.push_back(Method(retAddress, name));
}

/**
 *
 *
 * Remove the actively executing script method of the calling stack/method stack
 *
 *
 */
void Script::popActiveMethod() {
    if (functions.size() == 0) {
        ScriptError::msg("return from function requires stack popping, however method stack is empty");
        return;
    } else {
        functions.pop_back();
    }
}
/** Assign a global variable scope from Application layer 
 *  This means that a pointer address will be passed to the global.
 *  @param string name
 *  @param RegisteredVariable type will be a enum value
 *  @param void* address
 * 
 */
void Script::registerVariable(string name, RegisteredVariable type, void* address) {
    variables[name] = ScriptVariable(name, type, address);
}
/** Assign a global variable scope from runtime code:
 * 
 * @param name
 */
void Script::registerVariable(string name) {
    //Will register a global scope value but without a pointer address should
    //not be unregistered at the end of execution:
    variables[name] = ScriptVariable(name, true);
}
void Script::unregisterVariable(string name){
    map<string, ScriptVariable>::iterator it;
    it = variables.find(name);
    if (it != variables.end()) {
        variables.erase(it);
    }
}

/**
 *
 * Returns the method that is on top of the stack.
 *
 * It represents the method that is currently running, the
 * method the cpu is executing.
 *
 * If null is returned, it indicates that we are executing the "loader opcodes", which were
 * injected into the main script code and are responsible for the entry to function defined in our
 * main script code
 * @return
 */
Method* Script::getActiveMethod() {
    if (functions.size() == 0) {
        return NULL;
    } else {
        return &functions[functions.size()-1];
    }
}

/**
 * Searches the current method the script is executing for the variable indicated by string varName.
 * If such a variable is not found, searches registered script variables.  If no such variable is found
 * anywhere, null is returned
 * @param varName
 * @return
 */
ScriptVariable* Script::getVariable(string varName) {
    //Get a handle of the current executing method
    
    //cout << " ** getVariable -> " << varName << endl;
    
    Method* m = getActiveMethod();
    //sv points the to variable to be returned to caller
    ScriptVariable *sv = NULL;
    if (m != NULL) { //if there is a valid and active method
        //cout << " ** variable in method -> " << m->getName() << endl;
        m->renderScopeVars();
        
        sv = m->getVariable(varName); //search for the variable we are interested in
        if ( sv == NULL ) {	 //if not found, se variable to the return value of possible registered variable
            sv = getGlobalVariable(varName);
        }
    } else {
        //Note in scope so search in global scope
        sv = getGlobalVariable(varName);
    }
    return sv;
}
/**
 * Seraches variable is GLOBAL scope.
 * @param varName
 * @return
 */
ScriptVariable* Script::getGlobalVariable(string varName) {
    //------- Search in global scope:
    map<string, ScriptVariable>::iterator it;
    ScriptVariable *sv = NULL;
    it = variables.find(varName);
    if (it != variables.end()) {
        sv = &it->second; //return the address of the ScriptVariable
    }
    return sv;
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

bool Script::isSystemCall(string object, string functionName) {
    
    //Console print:
    if (functionName == "rep" || functionName == "print") {
        StackData sd = Stack::pop();
        if (sd.isNumber()) {
            ScriptConsole::print(ScriptConsole::toString(sd.getNumber()));
        } else {
            ScriptConsole::print(sd.getString());
        }
        return true;
    }
    
    //Hndle objects
    if (object != "NULL" && object != "null") {
        /**
         * Here, we find the variable denoted by 'object', then depending on the value of funcName depends
         * on the variables behavior
         */
        ScriptVariable* sv = getVariable(object);
        if (sv != NULL) {
            //return the length of a string
            if (functionName == "length") {
                if (sv->isNumber()) {
                    Stack::push(0);
                } else {
                    double len = (double)(sv->getStringValue().length());
                    Stack::push(len);
                }
            }
            //return substring of a string
            //definition of substring: object.substring(index, numberOfCharacters)
            if (functionName == "substr") {
                if (sv->isString()) {
                    Stack::render();
                    string value = sv->getStringValue();
                    StackData sb = Stack::pop();	//second argument first
                    StackData sa = Stack::pop();	//first argument
                    int a = sa.getNumber();
                    int b = sb.getNumber();
                    string sub = value.substr(a,b);
                    cout << "need to assign the value of " << sub << endl;
                    if (a >= 0 && a <= (int)value.size()-b-1) {	//make sure we are not out of bounds
                        Stack::push(sub);
                    } else {
                        Stack::push("null_range");
                    }
                }
            }
            return true;
        } else {
            ScriptError::msg("Unable to find object " + object + " for system call " + functionName);
            return false;
        }
    }
    return false;
}

string errors[] = {" ", "1 script object is null",
		"2 recursive call max out script contains error",
		"3 syntax error for function definition",
		"4 if-statement syntax error",
		"5 while-statement syntax error" };

bool Script::load(string filename) {
    int extensionIndex = filename.find_first_of(".");
    /**
     * -force scs extension
     * Check to make sure user has our script extension
     */
    if(extensionIndex != -1) {
        string extension = filename.substr(extensionIndex+1);
        if(extension != "scs"){	//solidus code script = scs
            ScriptError::msg("script extension \"" + extension + "\" does not match OpenWebQuery scripts[owq]");
            return false;
        } else {
            //continue normal execution
        }
    } else {
        ScriptError::msg("script file failed to load, no file extension provided");
        return false;
    }
    ifstream input;
    input.open(filename.c_str());

    if (!input) {
        ScriptError::msg("unable to open " + filename + " for loading");
        return false;
    } else {
        //SAVE hold code instruction
        Parser parser;
        string line;
        int ret = 0;
        while (!input.eof()) {
            getline(input, line);
            //clean up line a little,
            //tab characters give the script issues so we will remove it
            //ignore semicolons
            for (int i=0; i<(int)line.length(); i++) {
                if(line[i] == '\t'){
                    line[i] = ' ';
                }
                if(line[i] == ';'){
                    //line[i] = ' ';
                }
            }
            ret = parser.compile(this, line);
            if (ret != 0) {
                ScriptError::msg("compile error at line \"" + line + "\" : " + errors[ret]);
                return false;
            }
        }
        //RESTORE hold code instruction
    }
    return true;
}

Script::~Script() {

}