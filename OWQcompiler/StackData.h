/* 
 * File:   StackData.h
 * Author: shlomo hassid
 *
 */

#ifndef STACKDATA_H
#define	STACKDATA_H

#include <iostream>
#include <sstream>
#include <vector>
#include <map>

 /** the NULL value we use.
 *
 */
#ifndef OWQ_NAN
#define OWQ_NAN -3231307.6790
#endif

enum SDtype {
	SD_NULL,
	SD_STRING,
	SD_NUMBER,   
	SD_BOOLEAN,
	SD_OBJ,        
	SD_OBJpointer,    
	SD_ARRAY,         
	SD_RST,
	SD_GC,
};

class StackData {
    
	//My type:
	SDtype type;

	//Primitives:
	std::string svalue;
    double dvalue;
	int    bvalue;

	//Objects childs:
	bool isOwqObj; //Flag an object;
	bool isOwqArr; //Flag an array;
	std::map<std::string, StackData>* owqObj;
	std::vector<StackData>* owqArray;

	//Static stack pointers:
    bool   rst;
    int    rstPos;
	int    origin_index;

public:
    
	//Null Constructor:
    StackData();				    // Asign an undefined value
	void MutateToNull();

	//String Constructor:
	StackData(const std::string& value);	// Asign a string value
	void MutateToString(const std::string& value);

	//Number Constructor:
    StackData(double value);        // Asign a double value
	StackData(int value);           // Asign a double value
	void MutateToNumber(double value);
	void MutateToNumber(int value);

	//Boolean Constructor:
	StackData(bool value); // Asign a boolean value
	StackData(int value, bool valueBool); // Asign a boolean value
	StackData(const std::string& value, bool valueBool); // Asign a boolean value
	StackData(double value, bool valueBool); // Asign a boolean value
	void MutateToBoolean(bool value);
	void MutateToBoolean(int value);
	void MutateToBoolean(const std::string& value);
	void MutateToBoolean(double value);

	//Stack pointer Constructor:
    StackData(bool _rst, int _pos); // Asign a pointer stack value
    

    void setRstPos(int _rstPos);
	void setGc();
	void setOrigin(int _origin_index);

	SDtype getType();
	bool isOftype(SDtype t);
	bool isNull();
    bool isString();
    bool isNumber();
	bool isNumber(bool alsoBools);
	bool isBoolean();
    bool isRst();
	bool isGc();
    bool isRstPos(int pos);
    
    int         getOrigin();
	int         getRstPos();
    double&      getNumber();
	double      getNumber(bool alsoBools);
	int         getBoolean();
	bool        getRealBoolean();
	std::string& getString();
	std::string getAsString();
    
	std::string numberValueToString(double number);
	std::string numberValueToString(bool alsoBools);
	std::string numberValueToString();
	std::string& booleanValueToString();
    void render();

};

#endif	/* STACKDATA_H */

