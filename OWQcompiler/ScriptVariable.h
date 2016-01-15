/* 
 * File:   ScriptVariable.h
 * Author: shlomo hassid
 *
 */

#ifndef SCRIPTVARIABLE_H
#define	SCRIPTVARIABLE_H

#include <iostream>
#include <sstream>
#include <unordered_map>

#include "ScriptError.h"
#include "StackData.h"

#ifndef OWQ_NAN
#define OWQ_NAN -3231307.6790
#endif

enum RegisteredVariable { 
    REGISTERED_STRING,    //Holds a string 
    REGISTERED_DOUBLE,    //Holds a double
	REGISTERED_BOOLEAN,    //Holds a double
	GLOBAL_POINTER,
    GLOBAL_FLEX           //This is an execution global that is defined but not assigned 
};

class ScriptVariable {

	std::string name;
	StackData value;
    bool  isRegistered;
	int  hasPointers;
    void  *address;
	ScriptVariable* pointer;
    RegisteredVariable type;

public:

    //Constructors:
	ScriptVariable();
	ScriptVariable(std::string xName);
	ScriptVariable(std::string xName, StackData& sd);
	ScriptVariable(std::string xName, ScriptVariable* sv);
	ScriptVariable(std::string xName, RegisteredVariable type, void*  xAddress);

    //Get methods:
	std::string getName(); // Self name even if its a pointer.
	StackData getValue();
	StackData* getValuePointer();
	ScriptVariable* getPointer();

	//Validation check:
	bool inPointerPath(const std::string& xName);
	bool isRegister();
	bool isPointed();
    
	//Set value:
	bool setValue(StackData& sd);

	//Deref the pointer:
	void deref();

	//Set pointer flag:
	void setHasPointers(); // Will increase self even if its a pointer.
	void remHasPointers(); // Will decrease self even if its a pointer.

    //Render a variable for debugging:
	std::string renderVariable();

    //Destructor:
	virtual ~ScriptVariable();

};

#endif	/* SCRIPTVARIABLE_H */

