/* 
 * File:   StackData.h
 * Author: shlomo hassid
 *
 */

#ifndef STACKDATA_H
#define	STACKDATA_H

#include <iostream>
#include <sstream>

using namespace std;

class StackData {
    
    string svalue;
    double dvalue;

public:
    
    StackData(string value);
    StackData(double value);

    bool isString();
    bool isNumber();

    double getNumber();
    string numberValueToString();
    string getString();

    void render();
    virtual ~StackData();
};

#endif	/* STACKDATA_H */

