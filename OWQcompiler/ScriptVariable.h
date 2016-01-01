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

using namespace std;

enum RegisteredVariable { 
    REGISTERED_STRING,    //Holds a string 
    REGISTERED_DOUBLE,    //Holds a double
    GLOBAL_FLEX           //This is an execution global that is defined but not assigned 
};

class ScriptVariable {
    string name;
    string svalue;
    double dvalue;
    bool  isRegistered;
    void  *address;
    RegisteredVariable type;
public:
        //Constructors:
	ScriptVariable();
	ScriptVariable(string xName);
	ScriptVariable(string xName, double value);
	ScriptVariable(string xName, string value);
    ScriptVariable(string xName, bool reg);
	ScriptVariable(string xName, RegisteredVariable type, void*  xAddress);

        //Get methods:
	string getName();
	string getStringValue();
	double getNumberValue();
        
        //Set methods:
	void setNumberValue(double value);
	void setStringValue(string value);
	void setFromStackData(StackData& sd);
        
        //Check types:
	bool isString();
	bool isNumber();
        
        //Render a variable for debugging:
        string renderVariable();
        //Destructor:
	virtual ~ScriptVariable();
};

#endif	/* SCRIPTVARIABLE_H */

