
/* 
 * File:   StackData.cpp
 * Author: shlomo hassid
 *
 */

#include "Lang.h"
#include "StackData.h"

namespace Eowq {

	/** Construct a Stack Data that is undefined
	 *
	 * @param string|double value
	 */
	StackData::StackData() {
		type = SDtype::SD_NULL;
	}

	void StackData::MutateToNull() {
		type = SDtype::SD_NULL;
		owqval = NULLOWQVALUE;
	}

	bool StackData::MutateTo(StackData& sd) {
		switch (sd.getType()) {
		case SDtype::SD_NULL:
			MutateToNull();
			break;
		case SDtype::SD_BOOLEAN:
			MutateToBoolean(sd.getRealBoolean());
			break;
		case SDtype::SD_NUMBER:
			MutateToNumber(sd.getNumber());
			break;
		case SDtype::SD_STRING:
			MutateToString(sd.getString());
			break;
		case SDtype::SD_ARRAY:
			MutateToArray(sd.getArrayPointer());
			break;
		default:
			return false;
		}
		return true;
	}
	/** Construct a Stack Data of some type
	 *
	 * @param string|double value
	 */
	 //A STRING:
	StackData::StackData(const std::string& value) {
		MutateToString(value);
	}

	void StackData::MutateToString(const std::string& value) {
		type = SDtype::SD_STRING;
		owqval = NULLOWQVALUE;
		owqval.svalue = value;
	}

	//A NUMBER from double:
	StackData::StackData(double value) {
		MutateToNumber(value);
	}
	//A NUMBER from integer:
	StackData::StackData(int value) {
		MutateToNumber((double)value);
	}
	void StackData::MutateToNumber(int value) {
		MutateToNumber((double)value);
	}
	void StackData::MutateToNumber(double value) {
		type = SDtype::SD_NUMBER;
		owqval = NULLOWQVALUE;
		owqval.dvalue = value;
	}

	/** A special internal type Called RST which indicates a static internal pointer;
	 *
	 * @param boolean _rst
	 * @param integer _rstPos
	 */
	StackData::StackData(bool _rst, int _rstPos) {
		type = SDtype::SD_RST;
		owqval = NULLOWQVALUE;
		owqval.rst = _rst;
		owqval.rstPos = _rstPos;
		setRstPos(_rstPos);
	}
	/** A boolean value constructor will be an integer but set to a boolean container;
	*
	* @param int|string|double value
	* @param bool valueBool
	*/
	StackData::StackData(bool value) {
		MutateToBoolean(value);
	}
	StackData::StackData(int value, bool valueBool) {
		MutateToBoolean(value);
	}
	StackData::StackData(double value, bool valueBool) {
		MutateToBoolean(value);
	}
	StackData::StackData(const std::string& value, bool valueBool) {
		MutateToBoolean(value);
	}
	void StackData::MutateToBoolean(bool value) {
		type = SDtype::SD_BOOLEAN;
		owqval = NULLOWQVALUE;
		owqval.bvalue = value ? 1 : 0;
	}
	void StackData::MutateToBoolean(int value) {
		MutateToBoolean(value == 0 ? false : true);
	}
	void StackData::MutateToBoolean(const std::string& value) {
		MutateToBoolean(value == Lang::dicLangValue_true_lower || value == Lang::dicLangValue_true_upper ? 1 : 0);
	}
	void StackData::MutateToBoolean(double value) {
		MutateToBoolean(value > 0 ? true : false);
	}

	//Array constructor:
	StackData::StackData(std::vector<StackData>* arrayPointer) {
		MutateToArray(arrayPointer);
	}
	void StackData::MutateToArray(std::vector<StackData>* arrayPointer) {
		type = SDtype::SD_ARRAY;
		owqval = NULLOWQVALUE;
		owqval.isOwqArr = true;
		owqval.owqArray = arrayPointer;
	}
	StackData* StackData::traverseInArray(int* path, int index) {
		if (index > -1) {
			//Invert ele index if negative:
			int ele = path[index] >= 0 ? path[index] : (int)owqval.owqArray->size() + path[index];
			if (isArray() && ele >= 0 && (int)owqval.owqArray->size() > ele) {
				return owqval.owqArray->at(ele).traverseInArray(path, index - 1);
			} else {
				return nullptr;
			}
		}
		return this;
	}
	bool StackData::arrayPush(StackData& sd) {
		if (isArray()) {
			owqval.owqArray->push_back(sd);
			return true;
		}
		return false;
	}
	/** Set the internal static position of the data;
	 *
	 * @param integer _rstPos
	 */
	void StackData::setRstPos(int _rstPos) {
		owqval.rstPos = _rstPos;
	}
	/** The stack position that the stack data is placed in
	 *
	 */
	void StackData::setOrigin(int _origin_index) {
		owqval.origin_index = _origin_index;
	}
	int StackData::getOrigin() {
		return owqval.origin_index;
	}

	/** Get the owq stackdata type
	*
	*/
	SDtype StackData::getType() {
		return type;
	}
	/** checks if the stack data is of type
	 *
	 */
	bool StackData::isOftype(SDtype t) {
		return type == t ? true : false;
	}
	/** Check if a Stack Data is of type number (double)
	*
	* @return boolean
	*/
	bool StackData::isNull() {
		return type == SDtype::SD_NULL ? true : false;
	}
	/** Check if a Stack Data is of type number (double)
	 *
	 * @return boolean
	 */
	bool StackData::isNumber() {
		return type == SDtype::SD_NUMBER ? true : false;
	}
	bool StackData::isNumber(bool alsoBools) {
		bool numCheck = isNumber();
		bool boolCheck = isBoolean();
		return numCheck || boolCheck ? true : false;
	}
	/** Check if a Stack Data is of type string
	 *
	 * @return boolean
	 */
	bool StackData::isString() {
		return type == SDtype::SD_STRING ? true : false;
	}
	/** Check if a Stack Data is of type boolean
	*
	* @return boolean
	*/
	bool StackData::isBoolean() {
		return type == SDtype::SD_BOOLEAN ? true : false;
	}
	/** Check if a Stack Data is of type boolean
	*
	* @return boolean
	*/
	bool StackData::isArray() {
		return type == SDtype::SD_ARRAY ? true : false;
	}
	/** Checks if this Data has a Rst pointer
	 *
	 * @return boolean
	 */
	bool StackData::isRst() {
		return type == SDtype::SD_RST ? true : false;
	}
	/** Checks if this Data is at a rst position
	 *
	 * @param integer pos
	 * @return boolean
	 */
	bool StackData::isRstPos(int pos) {
		return pos == owqval.rstPos ? true : false;
	}
	/** get the rst integer value
	 *
	 */
	int StackData::getRstPos() {
		return owqval.rstPos;
	}

	/* Gc seter and getter:
	 * Quick GC mutate don't redefine owqval
	 */
	bool StackData::isGc() {
		return type == SDtype::SD_GC ? true : false;
	}
	void StackData::setGc() {
		type = SDtype::SD_GC;
		owqval.rstPos = -1;
		owqval.rst = false;
	}
	/** Returns the Stack Data number value
	 *
	 * @param boolean alsoBools DEFAULT : FALSE
	 * @return double
	 */
	double& StackData::getNumber() {
		return owqval.dvalue;
	}
	double StackData::getNumber(bool alsoBools) {
		if (isBoolean() && alsoBools) {
			return owqval.bvalue;
		}
		return (double)owqval.dvalue;
	}
	/* Will return any value as its number value 
	 * 
	 * Number -> Number
	 * String -> string length
	 * Boolean -> 1 | 0
	 * Array   -> Element count.
	 * Null,GC,RST    -> 0
	 * 
	 */
	double StackData::getAsNumber() {
		switch (type) {
			case SDtype::SD_NUMBER:
				return owqval.dvalue;
			case SDtype::SD_BOOLEAN:
				return (double)owqval.bvalue;
			case SDtype::SD_STRING:
				return (double)owqval.svalue.length();
			case SDtype::SD_ARRAY:
				return (double)owqval.owqArray->size();
		}
		return 0.0;
	};

	/** Returns the Stack Data string value
	 *
	 * @return string
	 */
	std::string& StackData::getString() {
		return owqval.svalue;
	}
	/** Returns the Stack Data boolean value
	*
	* @return int -> 1, 0
	*/
	int StackData::getBoolean() {
		return owqval.bvalue;
	}
	/** Returns the Stack Data boolean value in real form
	*
	* @return bool
	*/
	bool StackData::getRealBoolean() {
		return (owqval.bvalue > 0) ? true : false;
	}
	/** Returns the Stack Data boolean value in real form
	*
	* @return bool
	*/
	std::vector<StackData>* StackData::getArrayPointer() {
		return owqval.owqArray;
	}
	/** Converts a double to string
	 * @param double number DEFAUT : current dvalue
	 * @return string
	 */
	std::string StackData::numberValueToString() {
		return numberValueToString(owqval.dvalue);
	}
	std::string StackData::numberValueToString(bool alsoBools) {
		if (isNumber()) {
			return numberValueToString();
		}
		return booleanValueToString();
	}
	std::string StackData::numberValueToString(double number) {
		//std::stringstream s;
		//s << number;
		//return s.str();
		double intpart;
		return (std::modf(number, &intpart) == 0.0) ? std::to_string((int)intpart) : std::to_string(number);
	}
	/** Converts a boolean to string
	*
	* @return string
	*/
	std::string& StackData::booleanValueToString() {
		if (owqval.bvalue > 0)
			return Lang::dicLangValue_true_upper;
		return Lang::dicLangValue_false_upper;
	}
	/** Converts an array to string representation
	*
	* @return string
	*/
	std::string StackData::arrayValueToString() {
		if (owqval.owqArray == nullptr)
			return Lang::dicLangValue_null_upper;
		return "Array" + Lang::dicLang_sBraketOpen + std::to_string((int)owqval.owqArray->size()) + Lang::dicLang_sBraketClose;
	}
	std::string StackData::getAsString() {
		if (type == SDtype::SD_NUMBER) {         // Print a number
			return numberValueToString();
		}
		else if (type == SDtype::SD_STRING) {  // Print a string
			return getString();
		}
		else if (type == SDtype::SD_BOOLEAN) {
			return booleanValueToString();
		}
		else if (type == SDtype::SD_ARRAY) {
			return arrayValueToString();
		}
		else if (type == SDtype::SD_RST) {     // Print a rst
			return Lang::dicLangValue_rst_upper;
		}
		else if (type == SDtype::SD_GC) {
			return Lang::dicLangValue_garbage_upper;
		}
		return Lang::dicLangValue_null_upper;
	}

	/** Render the Stack Data to the terminal
	 *  For debugger usage.
	 */
	void StackData::render() {
		bool printRst = true;
		if (isNumber()) {         // Print a number
			std::cout << getNumber();
		} else if (isString()) {  // Print a string
			std::cout << getString();
		} else if (isBoolean()) { // Print a boolean
			std::cout << booleanValueToString();
		} else if (isArray()) {   // Print an owq array
			std::cout << getAsString();
		} else if (isRst()) {     // Print a rst
			std::cout << Lang::dicLangValue_rst_upper;
		} else if (isGc()) {      // Print a GC
			std::cout << Lang::dicLangValue_garbage_upper;
		} else { // Print NULL
			std::cout << Lang::dicLangValue_null_upper;
			printRst = false;
		}
		//Handle output of rst positions:
		if (printRst && owqval.rstPos > -1) {
			std::cout << " :: P{ " << owqval.rstPos << " }";
		}
	}
}