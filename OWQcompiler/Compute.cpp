/* 
 * File:   Token.cpp
 * Author: shlomi
 * 
 * Created on 30 דצמבר 2015, 04:59
 */

#include "Compute.h"
#include "Stack.h"
#include "Lang.h"

bool Compute::flagPush = false;

/** Execution error messages:
 *
 */
const std::string Compute::execute_errors[] = {
	"", // Ex_OK
	"Instruction for return without active method", // Ex_RETURN
	"Unable to resolve variable name: ",  // Ex_VAR_RESOLVE
	"Unable to assign to native variable", // Ex_NVAR_ASN
	"Null stack extrcation at operation: " // Ex_NULL_STACK_EXTRACTION
	"Increment / decrement operation un supported type - expected numeric: " // Ex_UNSUPPORTED_VAR_TYPE
};

const std::string Compute::execute_warn[] = {
	/* 0  */ "Tried to GTR with unsupported types: ",
	/* 1  */ "Tried to LSR with unsupported types: ",
	/* 2  */ "Expected AND evaluation of boolean expression to be defined or boolean.",
	/* 3  */ "Expected OR evaluation of boolean expression to be defined or boolean.",
	/* 4  */ "Break loop expected a positive numeric value or none - skipped loop break.",
	/* 5  */ "Break loop number is bigger than the nested loops.",
	/* 6  */ "Break condition expected a positive numeric value or none - skipped loop break.",
	/* 7  */ "Break condition number is bigger than the nested loops.",
	/* 8  */ "Expected result of condition expression to be boolean or defined.",
	/* 9  */ "Tried to execute Adition with unsupported types: ",
	/* 10 */ "Tried to execute Subtract with unsupported types: ",
	/* 11 */ "Tried to execute Multiplication with unsupported types: ",
	/* 12 */ "Division by zero prevented, calculation aborted",
	/* 13 */ "Tried to execute Division with unsupported types: ",
	/* 14 */ "Tried to execute Exponent with unsupported types: ",
	/* 15 */ "Argument declaration requires execution of method",
	/* 16 */ "Re-declaration of variable in method",
	/* 17 */ "Current active function requires sufficient argument",
	/* 18 */ "Unable to find function: ",
	/* 19 */ "Pointer must point to a declared valid variable in pointer: ",
	/* 20 */ "Pointer self reference is not alowed (infinite reference): ",
	/* 21  */ "Tried to GTR or EQUAL with unsupported types: ",
	/* 22  */ "Tried to LSR or EQUAL with unsupported types: ",
};
Compute::Compute() {

}
Compute::~Compute() {

}
/** Performs a stack shift:
 *
 */
ExecReturn Compute::execute_shift() {
	Stack::ShiftTop(true);
	return ExecReturn::Ex_OK;
}
/** Performs a stack swap:
 *
 */
ExecReturn Compute::execute_swap() {
	Stack::Swap();
	return ExecReturn::Ex_OK;
}
/** Push to stack: 
 *getValuePointer
 */
ExecReturn Compute::execute_push(Instruction& xcode, Script* script) {
	if (xcode.isOperandString()) { //if operand is string
		if (xcode.operandHasQuote()) { //if this operand is in the form---> ["what is this a string literal"]
			Stack::push(*xcode.getOperand()); //save the string literal
		} else if (xcode.isRstPointer() && xcode.getPointer() > 0) { // We are handling with a static stack pointer
			Stack::push(StackData(true, xcode.getPointer())); //This will push the special type of RST
		} else if (xcode.isOperandBoolean()) {
			Stack::push(StackData(*xcode.getOperand(), true)); //This will push a boolean stack data
		} else if (xcode.isOperandNull()) {
			Stack::push(StackData());
		} else {
			//the operand is in a form -> someWord <- no quotes around it, meaning it is a name of variable
			//find that variable from the function, loops scoping
			ScriptVariable* sv = script->getVariable(*xcode.getOperand());
			//complain if variable is not found
			if (sv == nullptr) {
				ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_VAR_RESOLVE] + *xcode.getOperand());
				return ExecReturn::Ex_VAR_RESOLVE;
			} else {
				//else push the value
				Stack::push(*sv);
			}
		}
	} else {
		Stack::push(xcode.getNumber());
	}
	//if operand is a reference to some variable, push its value onto the stack
	if (xcode.getPointer() > 0) {
		Stack::setTopPointer(xcode.getPointer());
	}
	return ExecReturn::Ex_OK;
}
/** Function return pointer
 *
 */
ExecReturn Compute::execute_function_return(Instruction& xcode, Script* script, int& instructionPointer) {
	OWQScope *m = script->getActiveScope();	//get active scope
	if (m != nullptr) {
		//instructionPointer = m->getReturnAddress() - 1;	//minus one because execution loop automatically increase by 1
														//if functions stack size == 1, then we are at the end of the execution stack,
														//meaning that we we can stop executing the script
		//if ( script->functions.size() == 1 ) {
			//Should return an error.
		//}
	} else {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_RETURN]);
		return ExecReturn::Ex_RETURN;
	}
	//return instruction pops the method/function off the method stack
	script->popActiveScope();
	return ExecReturn::Ex_OK;
}
/** Assign value to a defined variable:
 *
 */
ExecReturn Compute::execute_variable_assignment(Instruction &xcode, Script *script) {
	StackData* sd = Stack::pop(0); //pop and get the value to assign
	ScriptVariable* sv = script->getVariable(*xcode.getOperand()); //get the variable we wish to sign to
	if (sv == NULL || sd == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_VAR_RESOLVE] + *xcode.getOperand());
		return ExecReturn::Ex_VAR_RESOLVE;
	} else {
		int originSD = sd->getOrigin();
		//assign the value
		if (!sv->setValue(*sd)) {
			ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NVAR_ASN]);
			return ExecReturn::Ex_NVAR_ASN;
		}
		//Remove from stack:
		Stack::eraseAt(originSD);
		Stack::runGC();
	}
	return ExecReturn::Ex_OK;
}
/** Perform a pointer assignment
*
*/
ExecReturn Compute::execute_pointer_assignment(Instruction &xcode, Script *script, int& instructionPointer) {
	StackData* sd = Stack::pop(0); //pop and get the value it wont be needed but we need to earase it.
	ScriptVariable* sv = script->getVariable(*xcode.getOperand()); //get the variable we wish to sign to
	int ret = 0;
	if (sv == NULL || sd == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_VAR_RESOLVE] + *xcode.getOperand());
		return ExecReturn::Ex_VAR_RESOLVE;
	}
	if (script->code[instructionPointer - 1].getCode() == ByteCode::PUSH
		&& script->code[instructionPointer - 1].isOperandString()
		&& !script->code[instructionPointer - 1].isRstPointer()
		&& !script->code[instructionPointer - 1].isOperandBoolean()
	) {
		ret = script->pointerVariable(*xcode.getOperand(), *script->code[instructionPointer - 1].getOperand());
	} else {
		ret = 1;
	}
	int originSD = sd->getOrigin();
	Stack::eraseAt(originSD);
	Stack::runGC();
	if (ret > 0) {
		if (ret == 1) {
			ScriptError::warn(execute_warn[19] + *xcode.getOperand());
		} else {
			ScriptError::warn(execute_warn[20] + *xcode.getOperand());
		}
	}
	return ExecReturn::Ex_OK;
}


/** Perform a greater than operation
 *
 */
ExecReturn Compute::execute_math_gtr(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(StackData(a->getNumber(true) > b->getNumber(true)));
		} else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(StackData(a->getNumber(true) > b->getString().length()));
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(StackData(a->getString().length() > b->getNumber(true)));
		} else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getString().length() > b->getString().length()));
		} else { //unsopurted
			ScriptError::warn(execute_warn[0] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a lesser than operation
*
*/
ExecReturn Compute::execute_math_lsr(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(StackData(a->getNumber(true) < b->getNumber(true)));
		} else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(StackData(a->getNumber(true) < b->getString().length()));
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(StackData(a->getString().length() < b->getNumber(true)));
		} else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getString().length() < b->getString().length()));
		} else { //unsopurted
			ScriptError::warn(execute_warn[1] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a greater than operation
*
*/
ExecReturn Compute::execute_math_gtre(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	}
	else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(StackData(a->getNumber(true) >= b->getNumber(true)));
		}
		else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(StackData(a->getNumber(true) >= b->getString().length()));
		}
		else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(StackData(a->getString().length() >= b->getNumber(true)));
		}
		else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getString().length() >= b->getString().length()));
		}
		else { //unsopurted
			ScriptError::warn(execute_warn[21] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a lesser than operation
*
*/
ExecReturn Compute::execute_math_lsre(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	}
	else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(StackData(a->getNumber(true) <= b->getNumber(true)));
		}
		else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(StackData(a->getNumber(true) <= b->getString().length()));
		}
		else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(StackData(a->getString().length() <= b->getNumber(true)));
		}
		else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getString().length() <= b->getString().length()));
		}
		else { //unsopurted
			ScriptError::warn(execute_warn[22] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a equal operation
*
*/
ExecReturn Compute::execute_math_cve(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(StackData(a->getNumber(true) == b->getNumber(true)));
		} else if (a->isNumber(true) && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getNumber(true) == b->getString().length()));
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(StackData(a->getString().length() == b->getNumber(true)));
		} else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getString() == b->getString()));
		} else {
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a not equal operation
*
*/
ExecReturn Compute::execute_math_cvn(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(StackData(a->getNumber(true) != b->getNumber(true)));
		} else if (a->isNumber(true) && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getNumber(true) != b->getString().length()));
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(StackData(a->getString().length() != b->getNumber(true)));
		} else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(StackData(a->getString() != b->getString()));
		} else {
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a type equal operation
*
*/
ExecReturn Compute::execute_math_cte(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->getType() == b->getType()) {
			Stack::push(StackData(true));
		} else {
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a type not equal operation
*
*/
ExecReturn Compute::execute_math_ctn(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->getType() != b->getType()) {
			Stack::push(StackData(true));
		} else {
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a and boolean operation
*
*/
ExecReturn Compute::execute_math_and(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (!a->isNumber(true) || !b->isNumber(true)) {
			ScriptError::warn(execute_warn[2]);
		} else if (a->getNumber(true) > 0 && b->getNumber(true) > 0) {
			Stack::push(StackData(true));
		} else {
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a por boolean operation
*
*/
ExecReturn Compute::execute_math_por(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (!a->isNumber(true) || !b->isNumber(true)) {
			ScriptError::warn(execute_warn[3]);
		} else if (a->getNumber(true) > 0 || b->getNumber(true) > 0) {
			Stack::push(StackData(true));
		} else {
			Stack::push(StackData(false));
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a add operation
*
*/
ExecReturn Compute::execute_math_add(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) {  //a = number AND b = number
			Stack::push(a->getNumber(true) + b->getNumber(true));
		} else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(a->numberValueToString(true) + b->getString());
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(a->getString() + b->numberValueToString(true));
		} else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(a->getString() + b->getString());
		} else { //Unsupprted.
			ScriptError::warn(execute_warn[9] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(0);
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a subtract operation
*
*/
ExecReturn Compute::execute_math_subtract(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(a->getNumber(true) - b->getNumber(true));
		} else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(a->getNumber(true) - b->getString().length());
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(a->getString().length() - b->getNumber(true));
		} else if (a->isString() && b->isString()) {                                                //a = string AND b = string
			Stack::push(a->getString().length() - b->getString().length());
		} else { //Unsupprted.
			ScriptError::warn(execute_warn[10] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(0);
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a subtract operation
*
*/
ExecReturn Compute::execute_math_mul(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) {  //a = number AND b = number
			Stack::push(a->getNumber(true) * b->getNumber(true));
		} else if (a->isNumber(true) && b->isString()) {  //a = number AND b = string
			Stack::push(a->getNumber(true) * b->getString().length());
		} else if (a->isString() && b->isNumber(true)) {  //a = string AND b = number
			Stack::push(a->getString().length() * b->getNumber(true));
		} else if (a->isString() && b->isString()) {  //a = string AND b = string
			Stack::push(a->getString().length() * b->getString().length());
		} else { //Unsupprted.
			ScriptError::warn(execute_warn[11] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(0);
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a division operation
*
*/
ExecReturn Compute::execute_math_divide(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (b->isString()) { //First make sure b is not of zero length string
			if (b->getString().length() == 0) {
				ScriptError::warn(execute_warn[12]);
				Stack::push(0);
				return ExecReturn::Ex_OK;
			}
		}
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			if (b->getNumber() == 0.0) {
				ScriptError::warn(execute_warn[12]);
				Stack::push(0);
				return ExecReturn::Ex_OK;
			}
			Stack::push(a->getNumber(true) / b->getNumber(true));
		} else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(a->getNumber(true) / b->getString().length());
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(a->getString().length() / b->getNumber(true));
		} else if (a->isString() && b->isString()) { //a = string AND b = string
			Stack::push(a->getString().length() / b->getString().length());
		} else { //Unsupprted.
			ScriptError::warn(execute_warn[13] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(0);
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a exponent operation
*
*/
ExecReturn Compute::execute_math_expon(Instruction &xcode) {
	StackData* b = Stack::pop(0);
	StackData* a = Stack::pop(1);
	if (a == nullptr || b == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		int originB = b->getOrigin();
		if (a->isNumber(true) && b->isNumber(true)) { //a = number AND b = number
			Stack::push(pow(a->getNumber(true), b->getNumber(true)));
		} else if (a->isNumber(true) && b->isString()) { //a = number AND b = string
			Stack::push(pow(a->getNumber(true), b->getString().length()));
		} else if (a->isString() && b->isNumber(true)) { //a = string AND b = number
			Stack::push(pow(a->getString().length(), b->getNumber(true)));
		} else if (a->isString() && b->isString()) { //a = string AND b = string 
			Stack::push(pow(a->getString().length(), b->getString().length()));
		} else { //Unsupprted.
			ScriptError::warn(execute_warn[14] + ScriptConsole::stackTypeName(a->getType()) + "," + ScriptConsole::stackTypeName(b->getType()));
			Stack::push(0);
		}
		//Remove from stack:
		Stack::eraseAt(originB);
		Stack::eraseAt(originA);
		Stack::runGC();
		//Set static pointer:
		if (xcode.getPointer() > 0) {
			Stack::setTopPointer(xcode.getPointer());
		}
	}
	return ExecReturn::Ex_OK;
}


//Variables operations:
ExecReturn Compute::execute_math_inc_dec(Instruction &xcode, Script *script) {
	ScriptVariable* sv = script->getVariable(*xcode.getOperand());
	//complain if variable is not found
	if (sv == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_VAR_RESOLVE] + *xcode.getOperand());
		return ExecReturn::Ex_VAR_RESOLVE;
	}
	if (!sv->getValuePointer()->isNumber()) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_UNSUPPORTED_VAR_TYPE] + *xcode.getOperand());
		return ExecReturn::Ex_UNSUPPORTED_VAR_TYPE;
	}
	switch (xcode.getCode()) {
		case ByteCode::INCR:
			// ++$
			sv->getValuePointer()->getNumber()++;
			if (!flagPush) { Stack::push(*sv); }
			break;
		case ByteCode::INCL:
			// $++
			if (!flagPush) { Stack::push(*sv); }
			sv->getValuePointer()->getNumber()++;
			break;
		case ByteCode::DECR:
			// --$
			sv->getValuePointer()->getNumber()--;
			if (!flagPush) { Stack::push(*sv); }
			break;
		case ByteCode::DECL:
			// $--
			if (!flagPush) { Stack::push(*sv); }
			sv->getValuePointer()->getNumber()--;
			break;
		default:
			if (!flagPush) { Stack::push(*sv); }
	}

	//Set static pointer if needed:
	if (xcode.getPointer() > 0 && !flagPush) {
		Stack::setTopPointer(xcode.getPointer());
	} else {
		flagPush = false;
	}
	return ExecReturn::Ex_OK;
}

/** Perform a done instruction
*
*/
ExecReturn Compute::execute_done_block(Instruction &xcode, Script *script, int& instructionPointer) {
	//I call the instruction "DONE" a --repeater because it functions to change the instruction pointer to the first                    
	//imediate LOOP instruction by stepping backwards from the current code address
	//when at done, step backwards to first occurance of LOOP
	if (*xcode.getOperand() == Lang::dicLangKey_loop_while || *xcode.getOperand() == Lang::dicLangKey_loop_for) {
		if (xcode.getJmpCache() == -1) {
			int nested = 0;
			for (int j = instructionPointer - 1; j > -1; j--) {	//step backwards
				if (script->code[j].getCode() == ByteCode::DONE && *xcode.getOperand() == *script->code[j].getOperand()) {
					nested++;
				} else if (nested > 0 && script->code[j].getCode() == ByteCode::LOOP && *xcode.getOperand() == *script->code[j].getOperand()) {
					nested--;
				} else if (nested == 0 && script->code[j].getCode() == ByteCode::LOOP && *xcode.getOperand() == *script->code[j].getOperand()) { //if the instruction has an instruction code that matches LOOP of the same operand
					instructionPointer = j; //set that instruction address to ip and break immediately
					xcode.setJmpCache(j);
					break;
				}
			}
		} else {
			instructionPointer = xcode.getJmpCache();
		}
		//clear the scope:
		script->getActiveScope()->l.resetScope(script);

	} else if (*xcode.getOperand() == Lang::dicLangKey_cond_if) {
		//do nothing
	} else if (*xcode.getOperand() == Lang::dicLangKey_cond_else) {
		//do nothing
	} else {
		//do nothing
	}
	return ExecReturn::Ex_OK;
}
/** Perform a break loop instruction
*
*/
ExecReturn Compute::execute_loop_break(Instruction &xcode, Script *script, int& instructionPointer) {
	StackData* a = Stack::pop(0);
	if (a == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		if (!a->isNumber() || a->getNumber() < 1) {
			ScriptError::warn(execute_warn[4]);
		} else {
			int nestedLoops = 0;
			int numberOfBreaks = (int)a->getNumber();
			int j = instructionPointer + 1;
			for (j; j < (int)script->code.size(); j++) { //scan for next Done
				ByteCode curBC = script->code[j].getCode();
				std::string cupOPr = *script->code[j].getOperand();
				std::string langIf = Lang::dicLangKey_cond_if;
				std::string langElse = Lang::dicLangKey_cond_else;
				if (curBC == ByteCode::CMP || curBC == ByteCode::ELE) { //if the instruction has an instruction code that matches LOOP OR IF BLOCKS AFTER the BLOCK avoid
					nestedLoops++;
				} else if (nestedLoops > 0 && curBC == ByteCode::DONE) {
					nestedLoops--;
				} else if (
					nestedLoops == 0
					&& curBC == ByteCode::DONE
					&& cupOPr != langIf
					&& cupOPr != langElse
					) {
					numberOfBreaks--;
				}
				if (numberOfBreaks == 0) {
					break;
				}
			}
			if (numberOfBreaks == 0) {
				instructionPointer = j;
				//Now safely exit scopes:
				//Yhis will make sure poiters are deref before poping:
				for (int i = (int)a->getNumber(); i > 0; i--) {
					OWQScope* scopeToRemove = script->getActiveScope();
					if (scopeToRemove != nullptr) {
						if (scopeToRemove->type == ScopeType::ST_LOOP) {
							scopeToRemove->l.resetScope(script);
						} else {
							scopeToRemove->m.resetScope(script);
						}
						//Remove the scope:
						script->popActiveScope();
					}
				}
			} else {
				ScriptError::warn(execute_warn[5]);
			}
		}
		//Remove from stack:
		Stack::eraseAt(originA);
		Stack::runGC();
	}
	return ExecReturn::Ex_OK;
}
/** Perform a break condition instruction
*
*/
ExecReturn Compute::execute_cond_break(Instruction &xcode, Script *script, int& instructionPointer) {
	StackData* a = Stack::pop(0);
	if (a == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		if (!a->isNumber() || a->getNumber() < 1) {
			ScriptError::warn(execute_warn[6]);
		} else {
			int nestedConds = 0;
			int numberOfBreaks = (int)a->getNumber();
			int j = instructionPointer + 1;
			for (j; j < (int)script->code.size(); j++) {	//scan for next Done
				ByteCode curBC = script->code[j].getCode();
				std::string cupOpr = *script->code[j].getOperand();
				if (curBC == ByteCode::CMP || curBC == ByteCode::ELE) { //if the instruction has an instruction code that matches LOOP OR IF BLOCKS AFTER the BLOCK avoid
					nestedConds++;
				} else if (nestedConds > 0 && curBC == ByteCode::DONE) {
					nestedConds--;
				} else if (
					nestedConds == 0
					&& curBC == ByteCode::DONE
					&& cupOpr != Lang::dicLangKey_loop_while
					) {
					numberOfBreaks--;
				}
				if (numberOfBreaks == 0) {
					break;
				}
			}
			if (numberOfBreaks == 0) {
				instructionPointer = j;
			} else {
				ScriptError::warn(execute_warn[7]);
			}
		}
		//Remove from stack:
		Stack::eraseAt(originA);
		Stack::runGC();
	}
	return ExecReturn::Ex_OK;
}
/** Perform a condtion result check
*
*/
ExecReturn Compute::execute_cond_cmp(Instruction &xcode, Script *script, int& instructionPointer) {
	//I call CMP as the gate keeper because it functions to determine if the body of a condition can be executed or
	//not depending of the value on the stack
	StackData* a = Stack::pop(0);	//used to overal condition of the while or if statement
									//1 - true : 0 - false OR boolean type
	if (a == nullptr) {
		ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
		return ExecReturn::Ex_NULL_STACK_EXTRACTION;
	} else {
		int originA = a->getOrigin();
		if (!a->isNumber(true)) {
			ScriptError::warn(execute_warn[8]);
		} else {
			if (a->getNumber(true) > 0) {
				
				//condition is true continue
				//if its true open a stack if its not created allready :
				if (*xcode.getOperand() == Lang::dicLangKey_loop_while && ( script->scope.size() == 0 || script->getActiveScope()->l.getPersonalAddress() != instructionPointer)) {
					script->pushLoopScope(instructionPointer);
				}

			} else {

				//Unset the scope if needed:
				if (*xcode.getOperand() == Lang::dicLangKey_loop_while && script->scope.size() > 0 && script->getActiveScope()->l.getPersonalAddress() == instructionPointer) {
						//it should be unset:
						script->popActiveScope();
				}
				
				//Match the respecive done for the jump:
				if (xcode.getJmpCache() == -1) {
					int matchCount = 0;
					std::string curCode = *script->code[instructionPointer].getOperand();
					for (int j = instructionPointer; j < script->getSize(); j++) {
						if (script->code[j].getCode() == ByteCode::CMP && *script->code[j].getOperand() == curCode) {
							++matchCount;
						}
						if (script->code[j].getCode() == ByteCode::DONE && *script->code[j].getOperand() == curCode) {
							--matchCount;
						}
						if (matchCount == 0) {
							instructionPointer = j;	//change instruction pointer to be at this instancce of done instruction
							xcode.setJmpCache(j);
							break;
						}
					}
				} else {
					instructionPointer = xcode.getJmpCache();
				}

				//Check if there is a else statement cause if there is we need to jump one after:
				if (*script->code[instructionPointer].getOperand() == Lang::dicLangKey_cond_if && instructionPointer + 1 < (int)script->code.size()) {
					if (script->code[instructionPointer + 1].getCode() == ByteCode::ELE) {
						//Jump to after target else
						instructionPointer++;
					}
				}
			}
		}
		//Remove from stack:
		Stack::eraseAt(originA);
		Stack::runGC();
	}
	return ExecReturn::Ex_OK;
}
/** Perform a condtion else jump
*
*/
ExecReturn Compute::execute_cond_else(Instruction &xcode, Script *script, int& instructionPointer) {
	//jump the block because the only way is through the IF and not directly:
	int matchCount = 0;
	int size = script->getSize();
	for (int j = instructionPointer; j < size; j++) {
		if (script->code[j].getCode() == ByteCode::ELE) {
			++matchCount;
		}
		if (script->code[j].getCode() == ByteCode::DONE && *script->code[j].getOperand() == Lang::dicLangKey_cond_else) {
			--matchCount;
		}
		if (matchCount == 0) {
			instructionPointer = j; //change instruction pointer to be at this instancce of done instruction
			break;
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a function declaration
*
*/
ExecReturn Compute::execute_function_create(Instruction &xcode, Script *script, int& instructionPointer) {
	//save the return address inside the newly created executing function
	//script->pushMethodScope(instructionPointer + 1, *xcode.getOperand());
	return ExecReturn::Ex_OK;
}
/** Perform a function arguments declaration
*
*/
ExecReturn Compute::execute_arguments_declaration(Instruction &xcode, Script *script) {
	OWQScope *s = script->getActiveScope();	//get active scope
	if (s == nullptr) {
		ScriptError::warn(execute_warn[15]);
	} else {
		StackData* sd = Stack::pop(0);
		if (sd == nullptr) {
			ScriptError::fatal(execute_errors[(int)ExecReturn::Ex_NULL_STACK_EXTRACTION] + xcode.toString());
			return ExecReturn::Ex_NULL_STACK_EXTRACTION;
		} else {
			//int originSD = sd->getOrigin();
			//if (!s->addVariable(*xcode.getOperand(), *sd)) {
				//ScriptError::warn(execute_warn[16]);
			//}
			//Remove from stack:
			//Stack::eraseAt(originSD);
			//Stack::runGC();
		}
	}
	return ExecReturn::Ex_OK;
}
/** Perform a function arguments count check
*
*/
ExecReturn Compute::execute_arguments_count_check(Instruction &xcode, Script *script) {
	if (xcode.getNumber() > Stack::size()) {
		ScriptError::warn(execute_warn[17]);
	}
	return ExecReturn::Ex_OK;
}
/** Perform a variable declaration
*
*/
ExecReturn Compute::execute_variable_declaration(Instruction &xcode, Script *script) {
	bool ret;
	ret = script->registerVariable(*xcode.getOperand());
	return ExecReturn::Ex_OK;
}
/** Perform a variable unset
*
*/
ExecReturn Compute::execute_variable_unset(Instruction &xcode, Script *script) {
	bool ret = true;
	ret = script->unregisterVariable(*xcode.getOperand(), true);
	return ret ? ExecReturn::Ex_OK : ExecReturn::Ex_VAR_RESOLVE;
}
/** Perform a function execution call
*
*/
ExecReturn Compute::execute_function_call(Instruction &xcode, Script *script, int& instructionPointer) {
	std::string operand = *xcode.getOperand();
	std::string object;
	std::string method;
	int dotOperator = operand.find_last_of(Lang::dicLangKey_sub_object);
	if (dotOperator != -1) {
		object = operand.substr(0, dotOperator);
		method = operand.substr(dotOperator + 1);
	} else {
		method = operand;
	}
	if (!script->isSystemCall(object, method, xcode)) {
		int address = script->getFunctionAddress(operand);
		if (address == -1) {
			ScriptError::warn(execute_warn[18] + operand + Lang::dicLangKey_sub_object);
		} else {
			instructionPointer = address - 1;
		}
	}
	return ExecReturn::Ex_OK;
}