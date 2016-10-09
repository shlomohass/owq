/* 
 * File:   Instructions.h
 * Author: shlomo hassid
 *
 */

#ifndef INSTRUCTIONS_H
#define	INSTRUCTIONS_H

#include <iostream>
#include <sstream>
#include "ScriptVariable.h"

namespace Eowq
{
	enum OperandType {
		OPER_NEW,
		OPER_SRTING,
		OPER_NUMBER,
		OPER_RSTPOINTER,
		OPER_BOOLEAN,
		OPER_NULL,
		OPER_VARIABLE
	};

	enum ByteCode {
		NOP,    //nothing appends
		RET,    //return from a function, change instruction pointer, pop off method from function stack
		ASN,    //assign value to variable
		ASNA,	//assign value to variable and add
		ASNS,   //assign value to variable and sub
		POI,    //assign pointer to variable

		GTR,    //greater
		LSR,    //lesser
		GTRE,   //greater equal
		LSRE,   //lesser equal

		LOOP,
		BRE,    //break loop
		BIF,    //break if statement;
		DONE,   //End of block
		EIF,    //end of function
		CMP,    //The conditions results looks one value back in the stack for Boolean result;
		AND,    //Both are positive;
		POR,    //One OF Two are positive;
		CVE,    //Does the values match! ==
		CVN,    //Does the values dont match! !=
		CTE,    //Does the types match! =~
		CTN,    //Does the types dont match! !~
		ELE,    //The ELSE of a conditions;
		PUSH,   //push the value of a variable onto the stack, push immediate string, or push number
		ADD,    //pop two items off stack and perform operation
		SUB,    //pop two items off stack and perform operation
		MULT,   //pop two items off stack and perform operation
		DIV,    //pop two items off stack and perform operation
		EXPON,  //pop two items off stack and perform operation
		FUNC,   //push a method onto the function stack
		DEF,    // Define a variable
		UNS,    // Unset a variable
		ARG,    //define a variable and assign it a value from the stack
		ARGC,
		CALL,

		INCL,
		INCR,
		DECL,
		DECR,

		ARD,    //Array definition.
		DPUSH,  //Prevent next push. -> this is for Garbage preventing on un assigned or chained returns and variable operations.
		SWA,    //Swap stack
		SHT     //Shift top on stack.

	};

	extern std::string byteCode[43];

	class Token;

	class Instruction {

		ByteCode		code;
		std::string		operand;
		
		bool			arrayPush;
		int             arrayTraverse;

		OperandType		operandType;
		int				jmpCache;
		int				staticPointer;
		bool			isRST;
		bool			containsQuotes;
		int             attachedObj;

	public:

		Instruction();
		Instruction(ByteCode inst);
		Instruction(ByteCode inst, const std::string& xOperand);
		Instruction(ByteCode inst, const std::string& xOperand, int pointer);
		Instruction(ByteCode inst, const Token& imptoken);
		Instruction(ByteCode inst, const Token& imptoken, int pointer);

		bool isArrayPush();
		bool isArrayTraverse();
		int  getArrayTraverse();
		
		bool isOperandNumber();
		bool isOperandString();
		bool isOperandBoolean();
		bool isOperandNull();
		bool operandHasQuote();

		void setAttachedObj(int set);
		int isAttachedObj();

		OperandType& getOperandType();
		OperandType& setOperandType();
		void setOperandType(OperandType type);

		void setPointer(int pointer);
		int getPointer();
		void setJmpCache(int i);
		int getJmpCache();
		bool isRstPointer();
		double getNumber();
		std::string& getString();

		std::string* getOperand();
		std::string& getOperandRef();
		ByteCode getCode();
		std::string toString();
		std::string byteCodeToShort();

	};
}
#endif	/* INSTRUCTIONS_H */

