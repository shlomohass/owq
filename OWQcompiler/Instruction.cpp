/**
 * Instruction.cpp
 * Author: shlomo hassid
 * 
 */

#include <stdlib.h>
#include "Instruction.h"
#include "Lang.h"
#include <math.h>

namespace Eowq {

	std::string byteCode[] = {
		"NOP",
		"RET",
		"ASN",
		"POI",
		"GTR",
		"LSR",
		"GTRE",
		"LSRE",
		"LOOP",
		"BRE",
		"BIF",
		"DONE",
		"EIF",
		"CMP",
		"AND",
		"POR",
		"CVE",
		"CVN",
		"CTE",
		"CTN",
		"ELE",
		"PUSH",
		"ADD",
		"SUB",
		"MULT",
		"DIV",
		"EXPON",
		"FUNC",
		"DEF",
		"UNS",
		"ARG",
		"ARGC_CHECK",
		"CALL",
		"INCL",
		"INCR",
		"DECL",
		"DECR",
		"DPUSH",
		"SWA",
		"SHT"
	};

	Instruction::Instruction() {
		code = ByteCode::NOP;
		containsQuotes = false;
		operandType = OperandType::OPER_NEW;
		jmpCache = -1;
		staticPointer = 0;
		isRST = false;
	}

	std::string Instruction::toString() {
		std::string display = byteCodeToShort() + "           ";
		display = display.substr(0, 5) + " ->  ";
		//display properly
		if (operandHasQuote()) {
			display += "\"";
		}
		display += operand;
		//display properly
		if (operandHasQuote()) {
			display += "\"";
		}
		display += "                ";
		display = display.substr(0, 20);
		//display instruction set static pointer:
		std::ostringstream convert;   // stream used for the conversion
		convert << staticPointer;
		display += "  :  SP[ " + convert.str() + " ]";
		return display;
	}

	std::string Instruction::byteCodeToShort() {
		switch (code) {
		case ByteCode::GTRE:  return "GTE";
		case ByteCode::LSRE:  return "LSE";
		case ByteCode::LOOP:  return "LOO";
		case ByteCode::DONE:  return "DON";
		case ByteCode::PUSH:  return "PUS";
		case ByteCode::MULT:  return "MUL";
		case ByteCode::EXPON: return "POW";
		case ByteCode::FUNC:  return "FUN";
		case ByteCode::ARGC:  return "ARC";
		case ByteCode::CALL:  return "CAL";
		case ByteCode::INCL:  return "INL";
		case ByteCode::INCR:  return "INR";
		case ByteCode::DECL:  return "DEL";
		case ByteCode::DECR:  return "DER";
		case ByteCode::DPUSH: return "DPU";
		default:
			return byteCode[code];
		}
	}

	Instruction::Instruction(ByteCode inst, const std::string& xOperand) {
		code = inst;
		operand = xOperand;
		staticPointer = 0;
		jmpCache = -1;
		isRST = false;
		operandType = OperandType::OPER_NEW;
		if (operand[0] == Lang::LangStringIndicator && operand[operand.size() - 1] == Lang::LangStringIndicator) {
			containsQuotes = true;
			//remove the quotes now
			operand.erase(0, 1);//erase beginning quotation
			operand.erase(operand.size() - 1, 1);//erase the ending quotation
		}
		else {
			containsQuotes = false;
		}
	}
	Instruction::Instruction(ByteCode inst, const std::string& xOperand, int pointer) {
		code = inst;
		operand = xOperand;
		jmpCache = -1;
		staticPointer = pointer;
		operandType = OperandType::OPER_NEW;
		if (operand[0] == Lang::LangStringIndicator && operand[operand.size() - 1] == Lang::LangStringIndicator) {
			containsQuotes = true;
			//remove the quotes now
			operand.erase(0, 1);//erase beginning quotation
			operand.erase(operand.size() - 1, 1);//erase the ending quotation
		}
		else {
			containsQuotes = false;
			if (operand == Lang::dicLangValue_rst_upper) {
				isRST = true;
			}
		}
	}

	Instruction::Instruction(ByteCode inst) {
		code = inst;
		jmpCache = -1;
		containsQuotes = false;
		staticPointer = 0;
		isRST = true;
		operandType = OperandType::OPER_NEW;
	}

	void Instruction::setPointer(int pointer) {
		staticPointer = pointer;
	}
	int Instruction::getPointer() {
		return staticPointer;
	}
	void Instruction::setJmpCache(int i) {
		jmpCache = i;
	}
	int Instruction::getJmpCache() {
		return jmpCache;
	}
	bool Instruction::isRstPointer() {
		return isRST;
	}
	bool isNumberic(char c) {
		bool ret = false;
		switch (c) {
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
		if (operand.length() == 0 || operandHasQuote()) {
			return false;
		}
		bool valid = true;
		bool dotFlag = false;
		bool eFlag = false;
		bool minCount = 0;
		bool plusCount = 0;
		if (operand[0] == '-' || isNumberic(operand[0])) {
			for (int i = 1; i < (int)operand.length(); i++) {
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
					if (minCount == 0 && i > 0 && operand[i - 1] == 'e') {
						minCount++;
						continue;
					}
					else {
						valid = false;
						break;
					}
				}
				if (operand[i] == '+') {
					if (plusCount == 0 && i > 0 && operand[i - 1] == 'e') {
						plusCount++;
						continue;
					}
					else {
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
		}
		else {
			return false;
		}
	}

	bool Instruction::isOperandString() {
		return !isOperandNumber();
	}
	bool Instruction::isOperandBoolean() {
		if ((operand[0] == Lang::dicLangValue_true_lower[0] && operand == Lang::dicLangValue_true_lower) ||
			(operand[0] == Lang::dicLangValue_true_upper[0] && operand == Lang::dicLangValue_true_upper) ||
			(operand[0] == Lang::dicLangValue_false_lower[0] && operand == Lang::dicLangValue_false_lower) ||
			(operand[0] == Lang::dicLangValue_false_upper[0] && operand == Lang::dicLangValue_false_upper)
			) {
			return true;
		}
		return false;
	}
	bool Instruction::isOperandNull() {
		if ((operand[0] == Lang::dicLangValue_null_lower[0] && operand == Lang::dicLangValue_null_lower) ||
			(operand[0] == Lang::dicLangValue_null_upper[0] && operand == Lang::dicLangValue_null_upper)
			) {
			return true;
		}
		return false;
	}
	bool Instruction::operandHasQuote() {
		return containsQuotes;
	}

	OperandType& Instruction::getOperandType() {
		return operandType;
	}
	OperandType& Instruction::setOperandType() {
		if (isOperandString()) { //if operand is string
			if (operandHasQuote()) { //if this operand is in the form---> ["what is this a string literal"]
				setOperandType(OperandType::OPER_SRTING);
			}
			else if (isRstPointer() && getPointer() > 0) { // We are handling with a static stack pointer
				setOperandType(OperandType::OPER_RSTPOINTER); //This will push the special type of RST
			}
			else if (isOperandBoolean()) {
				setOperandType(OperandType::OPER_BOOLEAN);
			}
			else if (isOperandNull()) {
				setOperandType(OperandType::OPER_NULL);
			}
			else {
				setOperandType(OperandType::OPER_VARIABLE);
			}
		} else {
			setOperandType(OperandType::OPER_NUMBER);
		}
		return operandType;
	}
	void Instruction::setOperandType(OperandType type) {
		operandType = type;
	}

	std::string* Instruction::getOperand() {
		return &operand;
	}
	std::string& Instruction::getOperandRef() {
		return operand;
	}
	ByteCode Instruction::getCode() {
		return code;
	}
	double Instruction::getNumber() {
		return atof(operand.c_str());
	}
	std::string& Instruction::getString() {
		return operand;
	}
}