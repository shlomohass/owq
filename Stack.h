/* 
 * File:   Stack.h
 * Author: shlomo hassid
 *
 */

#ifndef STACK_H
#define	STACK_H

#include "StackData.h"
#include "ScriptVariable.h"

#include <iostream>
#include <vector>

using namespace std;

class Stack {
    static vector<StackData> stack;
    
public:
    Stack();
    static void push(double data);
    static void push(string data);
    static void push(StackData data);
    static void push(ScriptVariable& data);
    static int  size();
    static StackData pop();
    static void Swap();
    static void ShiftTop();
    static void ShiftTop(int index);
    static StackData Shift();
    static void render();
    virtual ~Stack();
};

#endif	/* STACK_H */

