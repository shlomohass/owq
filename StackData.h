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
    bool   rst;
    int    rstPos;

public:
    
    StackData();
    StackData(string value);
    StackData(double value);
    StackData(bool _rst, int _pos);
    
    void setRstPos(int _rstPos);
    
    bool isString();
    bool isNumber();
    bool isRst();
    bool isRstPos(int pos);
    
    int    getRstPos();
    double getNumber();
    string getString();
    
    string numberValueToString(double number);
    string numberValueToString();

    void render();
    
    virtual ~StackData();
};

#endif	/* STACKDATA_H */

