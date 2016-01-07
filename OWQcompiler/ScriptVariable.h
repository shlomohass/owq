/* 
 * File:   ScriptVariable.h
 * Author: shlomo hassid
 *
 */

#ifndef SCRIPTVARIABLE_H
#define	SCRIPTVARIABLE_H

#include <iostream>
#include <sstream>

#include "ScriptError.h"
#include "StackData.h"

#ifndef OWQ_NAN
#define OWQ_NAN -3231307.6790
#endif

enum RegisteredVariable { 
    REGISTERED_STRING,    //Holds a string 
    REGISTERED_DOUBLE,    //Holds a double
	REGISTERED_BOOLEAN,    //Holds a double
    GLOBAL_FLEX           //This is an execution global that is defined but not assigned 
};

class ScriptVariable {

	std::string name;
	StackData value;
    bool  isRegistered;
    void  *address;
    RegisteredVariable type;

public:

    //Constructors:
	ScriptVariable();
	ScriptVariable(std::string xName);
	ScriptVariable(std::string xName, StackData& sd);
	ScriptVariable(std::string xName, RegisteredVariable type, void*  xAddress);

    //Get methods:
	std::string getName();
	StackData getValue();

    //Set value:
	bool setValue(StackData& sd);

    //Render a variable for debugging:
	std::string renderVariable();

    //Destructor:
	virtual ~ScriptVariable();

};

#endif	/* SCRIPTVARIABLE_H */

