
/* 
 * File:   StackData.cpp
 * Author: shlomo hassid
 *
 */

#include "StackData.h"

/** Construct a Stack Data that is undefined
 * 
 * @param string|double value
 */
StackData::StackData() {
    dvalue = OWQ_NAN;
    svalue = "null";
	bvalue = -1;
    rstPos = -1;
    rst    = false;
}
/** Construct a Stack Data
 * 
 * @param string|double value
 */
StackData::StackData(string value) {
    dvalue = OWQ_NAN;
    svalue = value;
	bvalue = -1;
    rstPos = -1;
    rst    = false;
}
StackData::StackData(double value) {
	dvalue = value;
	svalue = "null";
	bvalue = -1;
	rstPos = -1;
	rst = false;
}
StackData::StackData(int value) {
    dvalue = (double)value;
    svalue = "null";
	bvalue = -1;
    rstPos = -1;
    rst    = false;
}
/** A special internal type Called RST which indicates a static internal pointer; 
 * 
 * @param boolean _rst
 * @param integer _rstPos
 */
StackData::StackData(bool _rst, int _rstPos) {
    dvalue = OWQ_NAN;
    svalue = "null";
	bvalue = -1;
    rst = _rst;
    setRstPos(_rstPos);
}
/** A boolean value constractor will be an integer but set to a boolean container;
*
* @param int|string|double value
* @param bool valueBool
*/
StackData::StackData(bool value) {
	dvalue = OWQ_NAN;
	svalue = "null";
	bvalue = (int)value;
	rstPos = -1;
	rst = false;
}
StackData::StackData(int value, bool valueBool) {
	dvalue = OWQ_NAN;
	svalue = "null";
	bvalue = value > 0 ? 1 : 0;
	rstPos = -1;
	rst = false;
}
StackData::StackData(double value, bool valueBool) {
	dvalue = OWQ_NAN;
	svalue = "null";
	bvalue = value > 0 ? 1 : 0;
	rstPos = -1;
	rst = false;
}
StackData::StackData(string value, bool valueBool) {
	dvalue = OWQ_NAN;
	svalue = "null";
	bvalue = value == "true" || value == "TRUE" ? 1 : 0;
	rstPos = -1;
	rst = false;
}
/** Destruct the Stack data element
 * 
 */
StackData::~StackData() {

}
/** Set the internal static position of the data; 
 * 
 * @param integer _rstPos
 */
void StackData::setRstPos(int _rstPos) {
    rstPos = _rstPos;
}
/** Check if a Stack Data is of type number (double)
 * 
 * @return boolean 
 */
bool StackData::isNumber() {
	return isNumber(false);
}
bool StackData::isNumber(bool alsoBools) {
	if (dvalue != OWQ_NAN && !alsoBools) {
		return true;
	}
	else if ((dvalue != OWQ_NAN || bvalue > -1) && alsoBools) {
		return true;
	}
	return false;
}
/** Check if a Stack Data is of type string  
 *
 * @return boolean
 */
bool StackData::isString() {
    if (svalue != "null") {
        return true;
    }
    return false;
}
/** Check if a Stack Data is of type boolean
*
* @return boolean
*/
bool StackData::isBoolean() {
	if (bvalue == 0 || bvalue == 1) {
		return true;
	}
	return false;
}
/** Checks if this Data has a Rst pointer
 * 
 * @return boolean
 */
bool StackData::isRst() {
    return rst;
}
/** Checks if this Data has a is at a rst position
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
/** Returns the Stack Data number value
 * 
 * @param boolean alsoBools DEFAULT : FALSE
 * @return double 
 */
double StackData::getNumber() {
    return getNumber(false);
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
string StackData::getString() {
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
string StackData::numberValueToString() {
    return numberValueToString(dvalue);
}
string StackData::numberValueToString(bool alsoBools) {
	if (isNumber()) {
		return numberValueToString();
	}
	else {
		return booleanValueToString();
	}
}
string StackData::numberValueToString(double number) {
    stringstream s;
    s << number;
    return s.str();
}
/** Converts a boolean to string
*
* @return string
*/
string StackData::booleanValueToString() {
	if (bvalue > 0) {
		return "TRUE";
	}
	return "FALSE";
}
string StackData::getAsString() {
	stringstream ss;
	if (isNumber()) {         // Print a number
		ss << getNumber();
	}
	else if (isString()) {  // Print a string
		ss << getString();
	}
	else if (isBoolean()) {
		ss << booleanValueToString();
	}
	else if (isRst()) {     // Print a rst
		ss << "RST";
	}
	else {                  // Print NULL
		ss << "NULL";
	}
	return ss.str();
}
/** Render the Stack Data to the terminal
 * 
 */
void StackData::render() {
    bool printRst = true;
    
    if (isNumber()) {         // Print a number
        cout << getNumber();
	} else if (isString()) {  // Print a string
		cout << getString();
	} else if (isBoolean()) {
		cout << booleanValueToString();
    } else if (isRst()) {     // Print a rst
        cout << "RST";
    } else {                  // Print NULL
        cout << "NULL";
        printRst = false;
    }
    //Handle output of rst positions:
    if (printRst && rstPos > -1) {
        cout << " :: P{ " << rstPos << " }";
    }
}