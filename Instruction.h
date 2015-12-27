/* 
 * File:   Instructions.h
 * Author: shlomo hassid
 *
 */

#ifndef INSTRUCTIONS_H
#define	INSTRUCTIONS_H

#include <string>
#include <iostream>

using namespace std;

enum ByteCode { 
    NOP,    //nothing appends
    RET,    //return from a function, change instruction pointer, pop off method from function stack
    ASN,    //assign value to variable
    GTR,    //greater
    LSR,    //lesser
    LOOP, 
    DONE,
    EIF,    //end of function
    CMP,    //The conditions results looks one value back in the stack for Boolean result;
    AND,    //Both are positive;
    POR,    //One OF Two are positive;
    CVE,    //Does the values match! ==
    ELE,    //The ELSE of a conditions;
    PUSH,   //push the value of a variable onto the stack, push immediate string, or push number
    ADD,    //pop two items off stack and perform operation
    SUB,    //pop two items off stack and perform operation
    MULT,   //pop two items off stack and perform operation
    DIV,    //pop two items off stack and perform operation
    EXPON,  //pop two items off stack and perform operation
    FUNC,   //push a method onto the function stack
    DEF,    // Define a variable
    ARG,    //define a variable and assign it a value from the stack
    ARGC, 
    CALL,
    
    SWA,    //Swap stack
    SHT     //Shift top on stack.
};
extern string byteCode[26];
class Instruction {
    ByteCode code;
    string operand;
    bool  containsQuotes;
public:
    Instruction();
    Instruction(ByteCode inst);
    Instruction(ByteCode inst, string xOperand);

    bool isOperandNumber();
    bool isOperandString();
    bool operandHasQuote();
    double getNumber();
    string getString();

    string getOperand();
    ByteCode getCode();
    string toString();
    string byteCodeToShort();
    virtual ~Instruction();
};

#endif	/* INSTRUCTIONS_H */

