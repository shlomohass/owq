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
namespace Eowq
{
	class Stack {
		static std::vector<StackData> stack;

	public:
		Stack();
		static void push(double& data);
		static void push(double&& data);
		static void push(bool& data);
		static void push(bool&& data);
		static void push(int& data);
		static void push(int&& data);
		static void push(std::string& data);
		static void push(std::string&& data);
		static void push(StackData& data);
		static void push_rvalue_obj(StackData&& data);
		static void push(ScriptVariable& data);
		static void push(std::vector<StackData>* arrayPointer, double arrayName);

		static int  size();
		static StackData* pop();
		static StackData* pop(int offset);
		static StackData* extract(int pointer);
		static void eraseAt(int index);
		static void eraseAsGC(int index);
		static void runGC();
		static void setTopPointer(int pointer);
		static void Swap();
		static void ShiftTop();
		static void ShiftTop(int index);
		static StackData Shift();
		static void render();
		virtual ~Stack();
	};
}
#endif	/* STACK_H */

