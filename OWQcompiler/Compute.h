/* 
 * File:   Token.h
 * Author: shlomi
 *
 * Created on 30 דצמבר 2015, 04:59
 */

#ifndef COMPUTE_H
#define	COMPUTE_H

#include "Script.h"
#include "Instruction.h"
#include <iostream>

namespace Eowq
{

	class Script;

	class Compute {

		static const std::string execute_errors[];
		static const std::string execute_warn[];
	
	public:

		static bool flagPush;

		Compute();
		virtual ~Compute();

		//Compute methods - Asignment and stack:
		static ExecReturn execute_shift();
		static ExecReturn execute_swap();
		static ExecReturn execute_push(Instruction &xcode, Script *script);
		static ExecReturn execute_function_return(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_variable_assignment(Instruction &xcode, Script *script);
		static ExecReturn execute_pointer_assignment(Instruction &xcode, Script *script, int& instructionPointer);

		//Compute methods - Math and comparison:
		static ExecReturn execute_math_gtr(Instruction &xcode);			// >
		static ExecReturn execute_math_lsr(Instruction &xcode);			// <
		static ExecReturn execute_math_gtre(Instruction &xcode);		// >=
		static ExecReturn execute_math_lsre(Instruction &xcode);		// <=
		static ExecReturn execute_math_cve(Instruction &xcode);			// ==
		static ExecReturn execute_math_cvn(Instruction &xcode);			// !=
		static ExecReturn execute_math_cte(Instruction &xcode);			// =~
		static ExecReturn execute_math_ctn(Instruction &xcode);			// !~
		static ExecReturn execute_math_and(Instruction &xcode);			// &&
		static ExecReturn execute_math_por(Instruction &xcode);			// ||
		static ExecReturn execute_math_add(Instruction &xcode);			// +
		static ExecReturn execute_math_subtract(Instruction &xcode);	// -
		static ExecReturn execute_math_mul(Instruction &xcode);			// *
		static ExecReturn execute_math_divide(Instruction &xcode);		// /
		static ExecReturn execute_math_expon(Instruction &xcode);		// ^

		//Variables operations:
		static ExecReturn execute_math_inc_dec(Instruction &xcode, Script *script); // ++$

		//Compute methods - Loops and functions:
		static ExecReturn execute_done_block(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_loop_break(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_cond_break(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_cond_cmp(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_cond_else(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_function_create(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_arguments_declaration(Instruction &xcode, Script *script);
		static ExecReturn execute_arguments_count_check(Instruction &xcode, Script *script);
		static ExecReturn execute_variable_declaration(Instruction &xcode, Script *script);
		static ExecReturn execute_variable_unset(Instruction &xcode, Script *script);
		static ExecReturn execute_function_call(Instruction &xcode, Script *script, int& instructionPointer);

		//Objects and arrays:
		static ExecReturn execute_array_definition(Instruction &xcode, Script *script, int& instructionPointer);
		static ExecReturn execute_array_assignment(Instruction &xcode, Script *script, ScriptVariable* sv);
		static ExecReturn execute_array_extract_value(Instruction &xcode, Script *script, ScriptVariable* sv);
	};

}
#endif	/* COMPUTE_H */

