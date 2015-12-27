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

/**
 * The stack functions to only contains immediate values, never variable names or reference pointers to variables.
 *
 * Therefore, any string contained on the stack is a value only, not an alais for some variable.
 */
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
    static StackData Shift();
    static void render();
    virtual ~Stack();
};

#endif	/* STACK_H */

