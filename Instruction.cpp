/**
 * Instruction.cpp
 * Author: shlomo hassid
 * 
 */

#include <stdlib.h>
#include "Instruction.h"
#include <math.h>


string byteCode[] = { 
    "NOP", 
    "RET", 
    "ASN", 
    "GTR", 
    "LSR", 
    "LOOP", 
    "DONE",
    "EIF", 
    "CMP",
    "AND",
    "POR",
    "CVE",
    "ELE",
    "PUSH", 
    "ADD", 
    "SUB", 
    "MULT", 
    "DIV", 
    "EXPON", 
    "FUNC", 
    "DEF", 
    "ARG", 
    "ARGC_CHECK", 
    "CALL",
    "SWA",
    "SHT"
};

Instruction::Instruction() {
    code = ByteCode::NOP;
    operand = ".none.";
    containsQuotes = false;
}

string Instruction::toString() {
    string display = byteCodeToShort() + "\t ->  ";
    cout.precision(4);
    if (operand != ".none.") {
        //display properly
        if(operandHasQuote()) {
            display += "\"";
        }
        display += operand;
        //display properly
        if(operandHasQuote()) { 
            display += "\"";
        }
    }
    return display;
}

string Instruction::byteCodeToShort() {
    string ret;
    switch (code) {
        case LOOP: ret  = "LOO"; break;
        case DONE: ret  = "DON"; break;
        case PUSH: ret  = "PUS"; break;
        case MULT: ret  = "MUL"; break;
        case EXPON: ret = "POW"; break;
        case FUNC: ret  = "FUN"; break;
        case ARGC: ret  = "ARC"; break;
        case CALL: ret  = "CAL"; break;
        default:
            ret = byteCode[code];                                  
    }
    return ret;
}
Instruction::Instruction(ByteCode inst, string xOperand) {
    code = inst;
    operand = xOperand;
    if (operand[0] == '"' && operand[operand.size()-1]== '"') {
        containsQuotes =true;
        //remove the quotes now
        operand.erase(0,1);//erase beginning quotation
        operand.erase(operand.size()-1,1);//erase the ending quotation
    } else {
        containsQuotes = false;
    }
}
Instruction::Instruction(ByteCode inst) {
	code = inst;
	operand = ".none.";
	containsQuotes =false;
}
bool isNumberic(char c) {
    bool ret = false;
    switch(c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            ret = true;
            break;
    }
    return ret;
}
bool Instruction::isOperandNumber() {
    if (operand.length() == 0) {
        return false;
    }
    bool valid     = true;
    bool dotFlag   = false;
    bool eFlag     = false;
    bool minCount  = 0;
    bool plusCount = 0;
    if (operand[0] == '-' || isNumberic(operand[0])) {
        for(int i = 1; i< operand.length(); i++){
            if (operand[i] == '.' && !dotFlag) {
                dotFlag = true;
                continue;
            }
            if (operand[i] == '.' && dotFlag) {
                valid = false;
                break;
            }
            if (operand[i] == 'e' && !eFlag) {
                eFlag = true;
                continue;
            }
            if (operand[i] == 'e' && eFlag) {
                valid = false;
                break;
            }
            if (operand[i] == '-') {
                if (minCount == 0 && i > 0 && operand[i-1] == 'e') {
                    minCount++;
                    continue;
                } else {
                    valid = false;
                    break;
                }
            }
            if (operand[i] == '+') {
                if (plusCount == 0 && i > 0 && operand[i-1] == 'e') {
                    plusCount++;
                    continue;
                } else {
                    valid = false;
                    break;
                }
            }
            if (!isNumberic(operand[i])) {
                valid = false;
                break;
            } 
        }
        return valid;
    } else {
        return false;
    }
}

bool Instruction::isOperandString() {
    return !isOperandNumber();
}
bool Instruction::operandHasQuote() {
    return containsQuotes;
}
string Instruction::getOperand() {
    return operand;
}
ByteCode Instruction::getCode() {
    return code;
}
double Instruction::getNumber() {
    return atof(operand.c_str());
}
Instruction::~Instruction() {
	// TODO Auto-generated destructor stub
}


