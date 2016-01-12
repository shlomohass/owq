/* 
 * File:   Stack.cpp
 * Author: shlomo hassid
 * 
 */

#include "Stack.h"
#include "ScriptError.h"

/** Holds the stack
 */
std::vector<StackData> Stack::stack;

/** Construct the stack
 * 
 */
Stack::Stack() {
	stack.reserve(200);
}
/** Push a number to the stack
 * 
 * @param double data
 */
void Stack::push(double data) {
    push(StackData(data));
}
/** Push a string to the stack
 * 
 * @param string data
 */
void Stack::push(std::string data) {
    push(StackData(data));
}
/** Push a stack data to the stack
 * 
 * @param StackData data
 */
void Stack::push(StackData data) {
    stack.push_back(data);
}
/**  Push a variable value to the stack
 * 
 * @param ScriptVariable data
 */
void Stack::push(ScriptVariable& data) {
	if (data.getValuePointer()->getRstPos() > 0) {
		data.getValuePointer()->setRstPos(0);
	}
	stack.push_back(data.getValue());
}
/** Pop a value from the stack
 * 
 * @return StackData
 */
StackData* Stack::pop() {
	return pop(0);
}
StackData* Stack::pop(int offset) {
    if (stack.size() == 0) {
        ScriptError::msg("stack is zero as vm is instructed to pop value off the stack");
        return nullptr;
    }
	int size = stack.size() - 1 - offset;
	if (stack[size].isRst()) {
		int rstPos = stack[size].getRstPos();
		stack.at(size).setGc();
		return extract(rstPos);
	}
	stack[size].setOrigin(size);
	return &stack[size];
}
void Stack::setTopPointer(int pointer) {
    if (stack.size() > 0) {
        stack.back().setRstPos(pointer);
        return;
    } 
	std::cout << std::endl << "Error: tried to set pointer in empty stack" << std::endl;
}
StackData* Stack::extract(int pointer) {
    for (int i=(int)stack.size()-1; i > -1; i--) {
        if (stack[i].getRstPos() == pointer && !stack[i].isGc()) {
            if (stack[i].isRst()) {
				int rstPos = stack[i].getRstPos();
				stack.at(i).setGc();
                return extract(rstPos);
			} else {
				stack[i].setOrigin(i);
				return &stack[i];
			}
        }
    }
	std::cout << std::endl << "Error: stack pointer did not found nothing!" << std::endl;
    return nullptr;
}
/* Erase from stack position 
 *
 */
void Stack::eraseAt(int index) {
	//immediate erase
	if (index == -1) {
		stack.pop_back();
	}
	if ((int)stack.size() > index && index > -1 && stack[index].getOrigin() == index) {
		stack.erase(stack.begin() + index);
		return;
	}
	for (int i = (int)stack.size() - 1; i > -1; i--) {
		if (stack[i].getOrigin() == index) {
			stack.erase(stack.begin() + i);
			return;
		}
	}
	std::cout << std::endl << "Error: stack erase out of bound" << std::endl;
}
/* Erase from stack position
*
*/
void Stack::runGC() {
	for (int i = (int)stack.size() - 1; i > -1; i--) {
		if (stack[i].isGc()) {
			stack.erase(stack.begin() + i);
			return;
		}
	}
}
/** Swap top 2 value on the stack
 * 
 */
void Stack::Swap() {
    if (stack.size() > 1) {
        StackData* top1 = pop(0);
        StackData* top2 = pop(1);
		int origin1 = top1->getOrigin();
		int origin2 = top2->getOrigin();
        stack.push_back(*top1);
        stack.push_back(*top2);
		eraseAt(origin2);
		eraseAt(origin1);
		runGC();
    }
}
/** Shift the entire stack by one
 * 
 */
void Stack::ShiftTop() {
    ShiftTop(0);
}
/** shift a chunk of the stack 
 * 
 * @param int index
 */
void Stack::ShiftTop(int index) {
    if (stack.size() == 0) {
        return;
    }
    StackData* top = pop();
	int origin = top->getOrigin();
    int sep = index;
    stack.insert(stack.begin() + sep, *top);
	Stack::eraseAt(origin);
	runGC();
}
/** Shift pops the bottom value
 * 
 * @return StackData
 */
StackData Stack::Shift() {
    if (stack.empty()) {
        ScriptError::msg("stack is zero as vm is instructed to Shift a value off the stack");
        return StackData("NULL");
    }
    StackData sd = stack[0];	//the the first item on the stack
    stack.erase(stack.begin());	//remove the item on first place of the stack
    return sd;			//return that data
}
/** render the stack to the terminal
 *  
 */
void Stack::render() {
	std::cout << "     Stack("<< stack.size() << "):\n";
    for (int i=(int)stack.size()-1; i > -1; i--) {
		std::cout << "           [" << i << "] = ";
        stack[i].render();
		std::cout << std::endl;
    }
}
/** get the stack current size
 * 
 * @return integer
 */
int Stack::size() {
    return stack.size();
}

/** Destruct the stack Instance
 * 
 */
Stack::~Stack() {
    // TODO Auto-generated destructor stub
}

