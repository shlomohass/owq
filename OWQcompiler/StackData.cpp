
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
    dvalue = NAN;
    svalue = "null";
    rstPos = -1;
    rst    = false;
}
/** Construct a Stack Data
 * 
 * @param string|double value
 */
StackData::StackData(string value) {
    dvalue = NAN;
    svalue = value;
    rstPos = -1;
    rst    = false;
}
StackData::StackData(double value) {
    dvalue = value;
    svalue = "null";
    rstPos = -1;
    rst    = false;
}
/** A special internal type Called RST which indicates a static internal pointer; 
 * 
 * @param boolean _rst
 * @param integer _rstPos
 */
StackData::StackData(bool _rst, int _rstPos) {
    dvalue = NAN;
    svalue = "null";
    rst = _rst;
    setRstPos(_rstPos);
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
    if(dvalue != NAN){
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
 * @return double 
 */
double StackData::getNumber() {
    return dvalue;
}
/** Returns the Stack Data string value
 * 
 * @return string
 */
string StackData::getString() {
    return svalue;
}
/** Converts a double to string
 * @paran double number DEFAUT : current dvalue
 * @return string 
 */
string StackData::numberValueToString() {
    return numberValueToString(dvalue);
}
string StackData::numberValueToString(double number) {
    stringstream s;
    s << number;
    return s.str();
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