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
namespace Eowq
{
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

	class StackData;

	struct OWQvalue {

		//Primitives:
		double dvalue = OWQ_NAN;
		std::string svalue = "";
		int bvalue = -1;
		
		//Objects:
		bool isOwqObj = false;
		bool isOwqArr = false;
		std::map<std::string, StackData>* owqObj = nullptr;
		std::vector<StackData>* owqArray = nullptr;

		//Positions and origins:
		int rstPos = -1;
		bool rst = false;
		int origin_index = -1;

	};

	static OWQvalue NULLOWQVALUE = OWQvalue();
	

	class StackData {
		//My type:
		SDtype type;
		//Value container:
		OWQvalue owqval;

	public:

		//Null Constructor:
		StackData();				    // Asign an undefined value
		void MutateToNull();
		bool MutateTo(StackData& sd);

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

		//Array constructor:
		StackData(std::vector<StackData>* arrayPointer); // Asign an array pointer stack value
		void MutateToArray(std::vector<StackData>* arrayPointer);
		StackData* traverseInArray(int* path, int index);
		bool arrayPush(StackData& sd);

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
		bool isArray();
		bool isRst();
		bool isGc();
		bool isRstPos(int pos);

		int         getOrigin();
		int         getRstPos();
		double&     getNumber();
		double      getNumber(bool alsoBools);
		double      getAsNumber();
		int         getBoolean();
		bool        getRealBoolean();
		std::vector<StackData>* getArrayPointer();
		std::string& getString();
		std::string getAsString();

		std::string numberValueToString(double number);
		std::string numberValueToString(bool alsoBools);
		std::string numberValueToString();
		std::string& booleanValueToString();
		std::string arrayValueToString();
		void render();

	};
}
#endif	/* STACKDATA_H */

