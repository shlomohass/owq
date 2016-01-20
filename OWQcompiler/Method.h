/* 
 * File:   Method.h
 * Author: shlomo hassid
 *
 */

#ifndef METHOD_H
#define	METHOD_H

#include "Loop.h"
#include "ScriptVariable.h"

class Method : public Loop {

private:

	int retAddress;
    //Cached name just for debugging future!
	std::string name;

public:
    
    //Constructors:
	Method();
	Method(int address);
	Method(int address, int xRetAddress, std::string setName);

    //Return address:
	int getReturnAddress();
        
    //Return cached name:
	std::string* getName();

};

#endif	/* METHOD_H */

