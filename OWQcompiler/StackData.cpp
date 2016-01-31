
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
		MutateToNull();
	}
	void StackData::MutateToNull() {
		type = SDtype::SD_NULL;
		dvalue = OWQ_NAN;
		svalue = Lang::dicLangValue_null_upper;
		bvalue = -1;
		isOwqObj = false;
		isOwqArr = false;
		owqObj = nullptr;
		owqArray = nullptr;
		rstPos = -1;
		rst = false;
		origin_index = -1;
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
		dvalue = OWQ_NAN;
		svalue = value;
		bvalue = -1;
		isOwqObj = false;
		isOwqArr = false;
		owqObj = nullptr;
		owqArray = nullptr;
		rstPos = -1;
		rst = false;
		origin_index = -1;
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
		dvalue = value;
		svalue = Lang::dicLangValue_null_upper;
		bvalue = -1;
		isOwqObj = false;
		isOwqArr = false;
		owqObj = nullptr;
		owqArray = nullptr;
		rstPos = -1;
		rst = false;
		origin_index = -1;
	}

	/** A special internal type Called RST which indicates a static internal pointer;
	 *
	 * @param boolean _rst
	 * @param integer _rstPos
	 */
	StackData::StackData(bool _rst, int _rstPos) {
		type = SDtype::SD_RST;
		dvalue = OWQ_NAN;
		svalue = Lang::dicLangValue_null_upper;
		bvalue = -1;
		isOwqObj = false;
		isOwqArr = false;
		owqObj = nullptr;
		owqArray = nullptr;
		rst = _rst;
		setRstPos(_rstPos);
		origin_index = -1;
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
		dvalue = OWQ_NAN;
		svalue = Lang::dicLangValue_null_upper;
		bvalue = value ? 1 : 0;
		isOwqObj = false;
		isOwqArr = false;
		owqObj = nullptr;
		owqArray = nullptr;
		rstPos = -1;
		rst = false;
		origin_index = -1;
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

	/** Set the internal static position of the data;
	 *
	 * @param integer _rstPos
	 */
	void StackData::setRstPos(int _rstPos) {
		rstPos = _rstPos;
	}
	/** The stack position that the stack data is placed in
	 *
	 */
	void StackData::setOrigin(int _origin_index) {
		origin_index = _origin_index;
	}
	int StackData::getOrigin() {
		return origin_index;
	}

	/**
	*
	*/
	SDtype StackData::getType() {
		return type;
	}
	/**
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
		return pos == rstPos ? true : false;
	}
	int StackData::getRstPos() {
		return rstPos;
	}

	/* Gc seter and getter:
	 *
	 */
	bool StackData::isGc() {
		return type == SDtype::SD_GC ? true : false;
	}
	void StackData::setGc() {
		type = SDtype::SD_GC;
		rstPos = -1;
		rst = false;
	}
	/** Returns the Stack Data number value
	 *
	 * @param boolean alsoBools DEFAULT : FALSE
	 * @return double
	 */
	double& StackData::getNumber() {
		return dvalue;
	}
	double StackData::getNumber(bool alsoBools) {
		if (isBoolean() && alsoBools) {
			return bvalue;
		}
		return (double)dvalue;
	}
	/** Returns the Stack Data string value
	 *
	 * @return string
	 */
	std::string& StackData::getString() {
		return svalue;
	}
	/** Returns the Stack Data boolean value
	*
	* @return int -> 1, 0
	*/
	int StackData::getBoolean() {
		return bvalue;
	}
	/** Returns the Stack Data boolean value in real form
	*
	* @return bool
	*/
	bool StackData::getRealBoolean() {
		return (bvalue > 0) ? true : false;
	}
	/** Converts a double to string
	 * @param double number DEFAUT : current dvalue
	 * @return string
	 */
	std::string StackData::numberValueToString() {
		return numberValueToString(dvalue);
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
		if (bvalue > 0) {
			return Lang::dicLangValue_true_upper;
		}
		return Lang::dicLangValue_false_upper;
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
		else if (type == SDtype::SD_RST) {     // Print a rst
			return Lang::dicLangValue_rst_upper;
		}
		else if (type == SDtype::SD_GC) {
			return Lang::dicLangValue_garbage_upper;
		}
		return Lang::dicLangValue_null_upper;
	}

	/** Render the Stack Data to the terminal
	 *
	 */

	void StackData::render() {
		bool printRst = true;
		if (isNumber()) {         // Print a number
			std::cout << getNumber();
		}
		else if (isString()) {  // Print a string
			std::cout << getString();
		}
		else if (isBoolean()) {
			std::cout << booleanValueToString();
		}
		else if (isRst()) {     // Print a rst
			std::cout << Lang::dicLangValue_rst_upper;
		}
		else if (isGc()) {
			std::cout << Lang::dicLangValue_garbage_upper;
		}
		else {                  // Print NULL
			std::cout << Lang::dicLangValue_null_upper;
			printRst = false;
		}
		//Handle output of rst positions:
		if (printRst && rstPos > -1) {
			std::cout << " :: P{ " << rstPos << " }";
		}
	}
}