/* 
 * File:   Lang.cpp
 * Author: shlomi
 * 
 * Created on 10 דצמבר 2015, 17:15
 */

#include "Lang.h"

namespace Eowq {
	//Initialize maps:
	std::map<std::string, std::string> Lang::LangInverseDelimiter = std::map<std::string, std::string>{};
	std::map<std::string, std::string> Lang::LangInverseKeywords = std::map<std::string, std::string>{};
	std::map<std::string, std::string> Lang::LangInverseValues = std::map<std::string, std::string>{};
	std::map<std::string, int> Lang::LangInverseSystemLib = std::map<std::string, int>{};

	//Set Delimiters:
	std::map<std::string, std::string> Lang::LangDelimiter = {
		{ "string"            , "\""	},
		{ "string-esc"        , "\\"	},
		{ "space"             , " "		},
		{ "plus"              , "+"		},
		{ "minus"             , "-"		},
		{ "inc"			      , "++"	},
		{ "dec"				  , "--"	},
		{ "multi"             , "*"		},
		{ "divide"            , "/"		},
		{ "equal"             , "="		},
		{ "equal-add"         , "+="	},
		{ "equal-sub"         , "-="	},
		{ "pointer"           , "->"	},
		{ "braketOpen"        , "("		},
		{ "braketClose"       , ")"		},
		{ "bracesOpen"        , "{"		},
		{ "bracesClose"       , "}"		},
		{ "sBraketOpen"       , "["     },
		{ "sBraketClose"      , "]"     },
		{ "power"             , "^"		},
		{ "exclamation"       , "!"		},
		{ "greater"           , ">"		},
		{ "smaller"           , "<"		},
		{ "greater-equal"     , ">="	},
		{ "smaller-equal"     , "<="	},
		{ "comma"             , ","		},
		{ "c-tequal"          , "=~"	},
		{ "c-ntequal"         , "!~"	},
		{ "c-equal"           , "=="	},
		{ "c-nequal"          , "!="	},
		{ "and"               , "&&"	},
		{ "or"                , "||"	},
		{ "semicolon"         , ";"		},
		{ "macro-def"		  , "#"		},
		{ "macro-set"		  , ":"		}
	};

	//Set Keywords:
	std::map<std::string, std::string> Lang::LangKeywords = {
		{ "variable"     , "let"		},
		{ "unset"        , "unset"		},
		{ "sub-object"   , "."			},
		{ "cond-if"      , "if"			},
		{ "cond-else"    , "else"		},
		{ "cond-break"   , "breakif"	},
		{ "loop-while"   , "while"		},
		{ "loop-for"     , "for"		},
		{ "loop-break"   , "break"		},
		{ "function"     , "function"	},
		{ "return"       , "return"		}
	};

	//Set Values - set both smaller case and upper case:
	std::map<std::string, std::string> Lang::LangValues = {
		{ "true-lower"        , "true"	},
		{ "true-upper"        , "TRUE"	},
		{ "false-lower"       , "false" },
		{ "false-upper"       , "FALSE" },
		{ "null-lower"		  , "null"	},
		{ "null-upper"        , "NULL"	},

		{ "garbage-lower"     , "GC"	},
		{ "garbage-upper"     , "gc"	},
		{ "rst-lower"		  , "rst"	},
		{ "rst-upper"         , "RST"	}
	};


	//Set chars allowed as names:
	//if we do not define '"' as a character, no tokens will be generated for string quoetation marks
	std::vector<char> Lang::LangNamingAllowedChars = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'_'
	};

	//Set allowed extensions:
	std::vector<std::string> Lang::extensionLib = {
		".owq",
		".OWQ",
		".towq"
	};

	//Set systemCalls extensions:
	std::map<int, std::string> Lang::LangSystemLib = {
		{ 1,  "print"	  },
		{ 2,  "rep"		  },
		{ 3,  "length"	  },
		{ 4,  "type"	  },
		{ 5,  "isNull"	  },
		{ 6,  "isPointer" },
		{ 7,  "isPointed" },
		{ 8,  "substr"	  },
		{ 9,  "join"	  },
		{ 10, "sum"		  }
	};

	Lang::Lang() {
		//Inverse Delimiter:    
		for (auto const &ent1 : LangDelimiter) {
			LangInverseDelimiter.insert(std::pair<std::string, std::string>(ent1.second, ent1.first));
		}
		//Inverse Keywords:    
		for (auto const &ent2 : LangKeywords) {
			LangInverseKeywords.insert(std::pair<std::string, std::string>(ent2.second, ent2.first));
		}
		//Inverse Values naming:
		for (auto const &ent3 : LangValues) {
			LangInverseValues.insert(std::pair<std::string, std::string>(ent3.second, ent3.first));
		}
		//Inverse System functions:
		for (auto const &ent4 : LangSystemLib) {
			LangInverseSystemLib.insert(std::pair<std::string, int>(ent4.second, ent4.first));
		}

		//Populate the language definition:
		LangPopulate();
	}
	Lang::Lang(const Lang& orig) {

	}
	Lang::~Lang() {

	}


	char Lang::LangGeneralSpace = ' ';
	char Lang::LangStringIndicator = ' ';
	char Lang::LangSubObject = ' ';
	char Lang::LangFunctionOpenArguChar = ' ';
	char Lang::LangFunctionCloseArguChar = ' ';
	char Lang::LangBlockOpenChar = ' ';
	char Lang::LangBlockCloseChar = ' ';
	char Lang::LangArrayOpenChar = ' ';
	char Lang::LangArrayCloseChar = ' ';
	char Lang::LangArgumentSpacer = ' ';
	char Lang::LangStringEscape = ' ';
	char Lang::LangOperationEnd = ' ';
	char Lang::LangMacroIndicator = ' ';
	char Lang::LangMacroSetChar = ' ';

	std::string Lang::dicLang_space = "";
	std::string Lang::dicLang_plus = "";
	std::string Lang::dicLang_minus = "";
	std::string Lang::dicLang_inc = "";
	std::string Lang::dicLang_dec = "";
	std::string Lang::dicLang_multi = "";
	std::string Lang::dicLang_divide = "";
	std::string Lang::dicLang_equal = "";
	std::string Lang::dicLang_equalAdd = "";
	std::string Lang::dicLang_equalSub = "";
	std::string Lang::dicLang_pointer = "";
	std::string Lang::dicLang_braketOpen = "";
	std::string Lang::dicLang_braketClose = "";
	std::string Lang::dicLang_bracesOpen = "";
	std::string Lang::dicLang_bracesClose = "";
	std::string Lang::dicLang_sBraketOpen = "";
	std::string Lang::dicLang_sBraketClose = "";
	std::string Lang::dicLang_power = "";
	std::string Lang::dicLang_exclamation = "";
	std::string Lang::dicLang_greater = "";
	std::string Lang::dicLang_smaller = "";
	std::string Lang::dicLang_greater_equal = "";
	std::string Lang::dicLang_smaller_equal = "";
	std::string Lang::dicLang_comma = "";
	std::string Lang::dicLang_c_tequal = "";
	std::string Lang::dicLang_c_ntequal = "";
	std::string Lang::dicLang_c_equal = "";
	std::string Lang::dicLang_c_nequal = "";
	std::string Lang::dicLang_and = "";
	std::string Lang::dicLang_or = "";
	std::string Lang::dicLang_semicolon = "";

	//String based keywords:
	std::string Lang::dicLangKey_variable = "";
	std::string Lang::dicLangKey_unset = "";
	std::string Lang::dicLangKey_sub_object = "";
	std::string Lang::dicLangKey_cond_if = "";
	std::string Lang::dicLangKey_cond_else = "";
	std::string Lang::dicLangKey_cond_break = "";
	std::string Lang::dicLangKey_loop_while = "";
	std::string Lang::dicLangKey_loop_for = "";
	std::string Lang::dicLangKey_loop_break = "";
	std::string Lang::dicLangKey_function = "";
	std::string Lang::dicLangKey_return = "";

	//Values based naming:
	std::string Lang::dicLangValue_true_lower = "";
	std::string Lang::dicLangValue_true_upper = "";
	std::string Lang::dicLangValue_false_lower = "";
	std::string Lang::dicLangValue_false_upper = "";
	std::string Lang::dicLangValue_null_lower = "";
	std::string Lang::dicLangValue_null_upper = "";

	//Internal values naming:
	std::string Lang::dicLangValue_garbage_upper = "GC";
	std::string Lang::dicLangValue_garbage_lower = "gc";
	std::string Lang::dicLangValue_rst_upper = "RST";
	std::string Lang::dicLangValue_rst_lower = "rst";

	/** Populate the language to cache symbols:
	 *
	 */
	void Lang::LangPopulate() {

		//Char based:
		LangGeneralSpace = LangFindDelimiter("space")[0];
		LangStringIndicator = LangFindDelimiter("string")[0];
		LangSubObject = LangFindKeyword("sub-object")[0];
		LangFunctionOpenArguChar = LangFindDelimiter("braketOpen")[0];
		LangFunctionCloseArguChar = LangFindDelimiter("braketClose")[0];
		LangBlockOpenChar = LangFindDelimiter("bracesOpen")[0];
		LangBlockCloseChar = LangFindDelimiter("bracesClose")[0];
		LangArrayOpenChar = LangFindDelimiter("sBraketOpen")[0];
		LangArrayCloseChar = LangFindDelimiter("sBraketClose")[0];
		LangArgumentSpacer = LangFindDelimiter("comma")[0];
		LangStringEscape = LangFindDelimiter("string-esc")[0];
		LangOperationEnd = LangFindDelimiter("semicolon")[0];
		LangMacroIndicator = LangFindDelimiter("macro-def")[0];
		LangMacroSetChar = LangFindDelimiter("macro-set")[0];

		//String based:
		dicLang_space = LangFindDelimiter("space");
		dicLang_plus = LangFindDelimiter("plus");
		dicLang_minus = LangFindDelimiter("minus");
		dicLang_inc = LangFindDelimiter("inc");
		dicLang_dec = LangFindDelimiter("dec");
		dicLang_multi = LangFindDelimiter("multi");
		dicLang_divide = LangFindDelimiter("divide");
		dicLang_equal = LangFindDelimiter("equal");
		dicLang_equalAdd = LangFindDelimiter("equal-add");
		dicLang_equalSub = LangFindDelimiter("equal-sub");
		dicLang_pointer = LangFindDelimiter("pointer");
		dicLang_braketOpen = LangFindDelimiter("braketOpen");
		dicLang_braketClose = LangFindDelimiter("braketClose");
		dicLang_bracesOpen = LangFindDelimiter("bracesOpen");
		dicLang_bracesClose = LangFindDelimiter("bracesClose");
		dicLang_sBraketOpen = LangFindDelimiter("sBraketOpen");
		dicLang_sBraketClose = LangFindDelimiter("sBraketClose");
		dicLang_power = LangFindDelimiter("power");
		dicLang_exclamation = LangFindDelimiter("exclamation");
		dicLang_greater = LangFindDelimiter("greater");
		dicLang_smaller = LangFindDelimiter("smaller");
		dicLang_greater_equal = LangFindDelimiter("greater-equal");
		dicLang_smaller_equal = LangFindDelimiter("smaller-equal");
		dicLang_comma = LangFindDelimiter("comma");
		dicLang_c_tequal = LangFindDelimiter("c-tequal");
		dicLang_c_ntequal = LangFindDelimiter("c-ntequal");
		dicLang_c_equal = LangFindDelimiter("c-equal");
		dicLang_c_nequal = LangFindDelimiter("c-nequal");
		dicLang_and = LangFindDelimiter("and");
		dicLang_or = LangFindDelimiter("or");
		dicLang_semicolon = LangFindDelimiter("semicolon");

		//Keywords:
		dicLangKey_variable = LangFindKeyword("variable");
		dicLangKey_unset = LangFindKeyword("unset");
		dicLangKey_sub_object = LangFindKeyword("sub-object");
		dicLangKey_cond_if = LangFindKeyword("cond-if");
		dicLangKey_cond_else = LangFindKeyword("cond-else");
		dicLangKey_cond_break = LangFindKeyword("cond-break");
		dicLangKey_loop_while = LangFindKeyword("loop-while");
		dicLangKey_loop_for = LangFindKeyword("loop-for");
		dicLangKey_loop_break = LangFindKeyword("loop-break");
		dicLangKey_function = LangFindKeyword("function");
		dicLangKey_return = LangFindKeyword("return");

		//Values based naming:
		dicLangValue_true_lower = LangFindValueNaming("true-lower");
		dicLangValue_true_upper = LangFindValueNaming("true-upper");
		dicLangValue_false_lower = LangFindValueNaming("false-lower");
		dicLangValue_false_upper = LangFindValueNaming("false-upper");
		dicLangValue_null_lower = LangFindValueNaming("null-lower");
		dicLangValue_null_upper = LangFindValueNaming("null-upper");

		//Internal values naming:
		dicLangValue_garbage_lower = LangFindValueNaming("garbage-lower");
		dicLangValue_garbage_upper = LangFindValueNaming("garbage-upper");
		dicLangValue_rst_lower = LangFindValueNaming("rst-lower");
		dicLangValue_rst_upper = LangFindValueNaming("rst-upper");

	}
	/** Find the character that represent a delimiter name:
	 *
	 * @param string key
	 * @return char
	 */
	std::string Lang::LangFindDelimiter(const std::string& key) {
		if (LangHasKeyDelimiter(key)) {
			return LangDelimiter.at(key);
		}
		return "";
	}
	/** Check whether a key delimiter is set:
	 *
	 * @param string key key name
	 * @return bool
	 */
	bool Lang::LangHasKeyDelimiter(const std::string& key) {
		return LangDelimiter.count(key) == 1;
	}
	/** Find the string that represent a value name:
	*
	* @param string key
	* @return char
	*/
	std::string Lang::LangFindValueNaming(const std::string& key) {
		if (LangHasValueNaming(key)) {
			return LangValues.at(key);
		}
		return "";
	}
	/** Check whether a string value name is set:
	*
	* @param string key key name
	* @return bool
	*/
	bool Lang::LangHasValueNaming(const std::string& key) {
		return LangValues.count(key) == 1;
	}

	/** Find the int that represent a function name:
	*
	* @param string key
	* @return char
	*/
	int Lang::LangFindSystemLib(const std::string& key) {
		if (key.empty()) return -1;
		if (LangHasSystemLib(key)) {
			return LangInverseSystemLib.at(key);
		}
		return -1;
	}
	/** Check whether a string function name is set:
	*
	* @param string key key name
	* @return bool
	*/
	bool Lang::LangHasSystemLib(const std::string& key) {
		return LangInverseSystemLib.count(key) == 1;
	}


	/** Check whether a char delimiter is an actual register delimiter:
	 *
	 * @param string key key name
	 * @return bool
	 */
	bool Lang::LangIsDelimiter(const std::string& value) {
		if (value[0] == dicLang_space[0] && value == dicLang_space) return true;
		if (value[0] == dicLang_plus[0] && value == dicLang_plus) return true;
		if (value[0] == dicLang_minus[0] && value == dicLang_minus) return true;
		if (value[0] == dicLang_inc[0] && value == dicLang_inc) return true;
		if (value[0] == dicLang_dec[0] && value == dicLang_dec) return true;
		if (value[0] == dicLang_multi[0] && value == dicLang_multi) return true;
		if (value[0] == dicLang_divide[0] && value == dicLang_divide) return true;
		if (value[0] == dicLang_equal[0] && value == dicLang_equal) return true;
		if (value[0] == dicLang_equalAdd[0] && value == dicLang_equalAdd) return true;
		if (value[0] == dicLang_equalSub[0] && value == dicLang_equalSub) return true;
		if (value[0] == dicLang_pointer[0] && value == dicLang_pointer) return true;
		if (value[0] == dicLang_braketOpen[0] && value == dicLang_braketOpen) return true;
		if (value[0] == dicLang_braketClose[0] && value == dicLang_braketClose) return true;
		if (value[0] == dicLang_bracesOpen[0] && value == dicLang_bracesOpen) return true;
		if (value[0] == dicLang_bracesClose[0] && value == dicLang_bracesClose) return true;
		if (value[0] == dicLang_sBraketOpen[0] && value == dicLang_sBraketOpen) return true;
		if (value[0] == dicLang_sBraketClose[0] && value == dicLang_sBraketClose) return true;
		if (value[0] == dicLang_power[0] && value == dicLang_power) return true;
		if (value[0] == dicLang_exclamation[0] && value == dicLang_exclamation) return true;
		if (value[0] == dicLang_greater[0] && value == dicLang_greater) return true;
		if (value[0] == dicLang_smaller[0] && value == dicLang_smaller) return true;
		if (value[0] == dicLang_greater_equal[0] && value == dicLang_greater_equal) return true;
		if (value[0] == dicLang_smaller_equal[0] && value == dicLang_smaller_equal) return true;
		if (value[0] == dicLang_comma[0] && value == dicLang_comma) return true;
		if (value[0] == dicLang_c_tequal[0] && value == dicLang_c_tequal) return true;
		if (value[0] == dicLang_c_ntequal[0] && value == dicLang_c_ntequal) return true;
		if (value[0] == dicLang_c_equal[0] && value == dicLang_c_equal) return true;
		if (value[0] == dicLang_c_nequal[0] && value == dicLang_c_nequal) return true;
		if (value[0] == dicLang_and[0] && value == dicLang_and) return true;
		if (value[0] == dicLang_or[0] && value == dicLang_or) return true;
		if (value[0] == dicLang_semicolon[0] && value == dicLang_semicolon) return true;
		return false;
	}
	bool Lang::LangIsDelimiter(const char& value) {
		return LangIsDelimiter(std::string(1, value));
	}
	/** Checks if a char|string is a delimiter of comparison:
	 *
	 * @param char|string value
	 * @return boolean
	 *
	 */
	bool Lang::LangIsComparison(const char& value) {
		return LangIsComparison(std::string(1, value));
	}
	bool Lang::LangIsComparison(const std::string& value) {
		if (value[0] == dicLang_greater[0] && value == dicLang_greater) return true;
		if (value[0] == dicLang_smaller[0] && value == dicLang_smaller) return true;
		if (value[0] == dicLang_greater_equal[0] && value == dicLang_greater_equal) return true;
		if (value[0] == dicLang_smaller_equal[0] && value == dicLang_smaller_equal) return true;
		if (value[0] == dicLang_c_tequal[0] && value == dicLang_c_tequal) return true;
		if (value[0] == dicLang_c_ntequal[0] && value == dicLang_c_ntequal) return true;
		if (value[0] == dicLang_c_equal[0] && value == dicLang_c_equal) return true;
		if (value[0] == dicLang_c_nequal[0] && value == dicLang_c_nequal) return true;
		return false;
	}
	/** Checks if a char|string is a delimiter of a condition such as AND OR etc:
	 *
	 * @param char|string value
	 * @return boolean
	 *
	 */
	bool Lang::LangIsOfCondition(const char& value) {
		return LangIsOfCondition(std::string(1, value));
	}
	bool Lang::LangIsOfCondition(const std::string& value) {
		if (value[0] == dicLang_and[0] && value == dicLang_and) return true;
		if (value[0] == dicLang_or[0] && value == dicLang_or) return true;
		return false;
	}
	/** Find the string that represent a keyword name (key):
	 *
	 * @param string key
	 * @return string
	 */
	std::string Lang::LangFindKeyword(const std::string& key) {
		if (LangHasKeyKeyword(key)) {
			return LangKeywords.at(key);
		}
		return "";
	}
	/** Check whether a key keyword is set:
	 *
	 * @param string key key name
	 * @return bool
	 */
	bool Lang::LangHasKeyKeyword(const std::string& key) {
		return LangKeywords.count(key) == 1;
	}
	/** Check whether a string is an actual register keyword:
	 *
	 * @param string key key name
	 * @return bool
	 */
	bool Lang::LangIsKeyword(const std::string& value) {
		return LangInverseKeywords.count(value) == 1;
	}
	/** Checks whether a letter is allowed as function or variable name:
	 *
	 * @param char|string[0] value
	 * @return bool
	 */
	bool Lang::LangIsNamingAllowed(const char& value) {
		if (find(LangNamingAllowedChars.begin(), LangNamingAllowedChars.end(), value) != LangNamingAllowedChars.end()) {
			return true;
		}
		return false;
	}
	bool Lang::LangIsNamingAllowed(const std::string& value) {
		if (find(LangNamingAllowedChars.begin(), LangNamingAllowedChars.end(), value[0]) != LangNamingAllowedChars.end()) {
			return true;
		}
		return false;
	}


	/*
	 * PRINTING STUFF:
	 */
	void Lang::printHeader(std::string headername) {
		std::cout << std::endl << "-------------------------------------------------------------------" << std::endl;
		std::cout << "| OWQ Debugger -> " << headername << std::endl;
		printSepLine(2);
	}
	void Lang::printSepLine(int breaks) {
		std::cout << "-------------------------------------------------------------------";
		for (int i = 0; i < breaks; i++) {
			std::cout << std::endl;
		}
	}
	void Lang::printEmpLine(int breaks) {
		for (int i = 0; i < breaks; i++) {
			std::cout << std::endl;
		}
	}

}