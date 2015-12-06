/* 
 * File:   Method.h
 * Author: shlomo hassid
 *
 */

#ifndef METHOD_H
#define	METHOD_H


#include "ScriptVariable.h"
#include <vector>

using namespace std;

class Method {
	vector<ScriptVariable> var;
	int retAddress;
        //Cached name just for debugging future!
        string name; 
public:
    
        //Constructors:
	Method();
	Method(int xRetAddress, string setName);
        
        //Destructor:
	virtual ~Method();
        
        //Push method variables by types:
	void addVariable(string name, string value);
	void addVariable(string name, double value);
	void addVariable(string name); //With NAN
        
        //Return address:
	int getReturnAddress();
        
        //Return cached name:
	string getName();
        
        //Get a scoped method variable:
	ScriptVariable *getVariable(string name);
        
        //Render all registered variables:
        void renderScopeVars();
};

#endif	/* METHOD_H */

