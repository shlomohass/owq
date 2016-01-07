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

class Stack {
    static std::vector<StackData> stack;
    
public:
    Stack();
    static void push(double data);
    static void push(std::string data);
    static void push(StackData data);
    static void push(ScriptVariable& data);
    static int  size();
    static StackData pop();
    static StackData extract(int pointer);
    static void setTopPointer(int pointer);
    static void Swap();
    static void ShiftTop();
    static void ShiftTop(int index);
    static StackData Shift();
    static void render();
    virtual ~Stack();
};

#endif	/* STACK_H */

