/* 
 * File:   Method.h
 * Author: shlomo hassid
 *
 */

#ifndef METHOD_H
#define	METHOD_H


#include "ScriptVariable.h"
#include <vector>

class Method {

	std::vector<ScriptVariable> var;
	int retAddress;
    //Cached name just for debugging future!
	std::string name;

public:
    
    //Constructors:
	Method();
	Method(int xRetAddress, std::string setName);
        
    //Destructor:
	virtual ~Method();
        
    //Push method variables by types:
	bool addVariable(std::string name, StackData& sd);
	bool addVariable(std::string name); //With NAN
        
    //Return address:
	int getReturnAddress();
        
    //Return cached name:
	std::string getName();
        
    //Get a scoped method variable:
	ScriptVariable *getVariable(std::string name);
	bool hasVariable(std::string name);

    //Render all registered variables:
    void renderScopeVars();

};

#endif	/* METHOD_H */

