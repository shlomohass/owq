/* 
 * File:   Instructions.h
 * Author: shlomo hassid
 *
 */

#ifndef INSTRUCTIONS_H
#define	INSTRUCTIONS_H

#include <iostream>
#include <sstream>

enum ByteCode { 
    NOP,    //nothing appends
    RET,    //return from a function, change instruction pointer, pop off method from function stack
    ASN,    //assign value to variable
    GTR,    //greater
    LSR,    //lesser
    LOOP,
	BRE,    //break loop
	BIF,    //break if statement;
    DONE,   //End of block
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
extern std::string byteCode[28];
class Instruction {
    ByteCode		code;
    std::string		operand;
    int				staticPointer;
    bool			isRST;
    bool			containsQuotes;
public:
    
    Instruction();
    Instruction(ByteCode inst);
    Instruction(ByteCode inst, std::string xOperand);
    Instruction(ByteCode inst, std::string xOperand, int pointer);
    
    bool isOperandNumber();
    bool isOperandString();
	bool isOperandBoolean();
    bool operandHasQuote();
    void setPointer(int pointer);
    int getPointer();
    bool isRstPointer();
    double getNumber();
    std::string getString();

    std::string getOperand();
    ByteCode getCode();
    std::string toString();
    std::string byteCodeToShort();
    virtual ~Instruction();
};

#endif	/* INSTRUCTIONS_H */

