/* 
 * File:   Lang.cpp
 * Author: shlomi
 * 
 * Created on 10 דצמבר 2015, 17:15
 */

#include "Lang.h"

//Initialize maps:
std::map<std::string, std::string> Lang::LangInverseDelimiter = std::map<std::string, std::string>{};
std::map<std::string, std::string> Lang::LangInverseKeywords = std::map<std::string, std::string>{};

//Set Delimiters:
std::map<std::string, std::string> Lang::LangDelimiter = {
	{ "string"            , "\"" },
	{ "string-esc"        , "\\" },
    { "space"             , " " },
    { "plus"              , "+" },
    { "minus"             , "-" },
    { "multi"             , "*" },
    { "divide"            , "/" },
    { "equal"             , "=" },
	{ "pointer"           , "->" },
    { "braketOpen"        , "(" },
    { "braketClose"       , ")" },
    { "bracesOpen"        , "{" },
    { "bracesClose"       , "}" },
    { "power"             , "^" },
    { "exclamation"       , "!" },
    { "greater"           , ">" },
    { "smaller"           , "<" },
    { "comma"             , "," },
	{ "c-tequal"          , "=~" },
	{ "c-ntequal"         , "!~" },
    { "c-equal"           , "==" },
	{ "c-nequal"          , "!=" },
    { "and"               , "&&" },
    { "or"                , "||" },
    { "semicolon"         , ";" },
	{ "macro-def"		  , "#" },
	{ "macro-set"		  , ":" }
};

//Set Keywords:
std::map<std::string, std::string> Lang::LangKeywords = {
    {"variable"     , "let"},
    {"cond-if"      , "if"},
    {"cond-else"    , "else"},
	{"cond-break"   , "breakif" },
    {"loop-while"   , "while"},
    {"loop-do"      , "do"},
	{"loop-break"   , "break" },
	{"function"     , "function" },
    {"return"       , "return"},
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

Lang::Lang() {
    //Inverse Delimiter:    
    for(auto const &ent1 : LangDelimiter) {
        LangInverseDelimiter.insert(std::pair<std::string, std::string>(ent1.second, ent1.first));
    } 
    //Inverse Keywords:    
    for(auto const &ent2 : LangKeywords) {
        LangInverseKeywords.insert(std::pair<std::string, std::string>(ent2.second, ent2.first));
    }
	//Populate the language:
	LangPopulate();
}
Lang::Lang(const Lang& orig) {
	
}
Lang::~Lang() {

}


char Lang::LangGeneralSpace = ' ';
char Lang::LangStringIndicator = ' ';
char Lang::LangFunctionOpenArguChar = ' ';
char Lang::LangFunctionCloseArguChar = ' ';
char Lang::LangBlockOpenChar = ' ';
char Lang::LangBlockCloseChar = ' ';
char Lang::LangArgumentSpacer = ' ';
char Lang::LangStringEscape = ' ';
char Lang::LangOperationEnd = ' ';
char Lang::LangMacroIndicator = ' ';
char Lang::LangMacroSetChar = ' ';

std::string Lang::dicLang_space = "";
std::string Lang::dicLang_plus = "";
std::string Lang::dicLang_minus = "";
std::string Lang::dicLang_multi = "";
std::string Lang::dicLang_divide = "";
std::string Lang::dicLang_equal = "";
std::string Lang::dicLang_pointer = "";
std::string Lang::dicLang_braketOpen = "";
std::string Lang::dicLang_braketClose = "";
std::string Lang::dicLang_bracesOpen = "";
std::string Lang::dicLang_bracesClose = "";
std::string Lang::dicLang_power = "";
std::string Lang::dicLang_exclamation = "";
std::string Lang::dicLang_greater = "";
std::string Lang::dicLang_smaller = "";
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
std::string Lang::dicLangKey_cond_if = "";
std::string Lang::dicLangKey_cond_else = "";
std::string Lang::dicLangKey_cond_break = "";
std::string Lang::dicLangKey_loop_while = "";
std::string Lang::dicLangKey_loop_do = "";
std::string Lang::dicLangKey_loop_break = "";
std::string Lang::dicLangKey_function = "";
std::string Lang::dicLangKey_return = "";

/** Populate the language to cache symbols:
 *
 */
void Lang::LangPopulate() {

	//Char based:
	LangGeneralSpace = LangFindDelimiter("space")[0];
	LangStringIndicator = LangFindDelimiter("string")[0];
	LangFunctionOpenArguChar = LangFindDelimiter("braketOpen")[0];
	LangFunctionCloseArguChar = LangFindDelimiter("braketClose")[0];
	LangBlockOpenChar = LangFindDelimiter("bracesOpen")[0];
	LangBlockCloseChar = LangFindDelimiter("bracesClose")[0];
	LangArgumentSpacer = LangFindDelimiter("comma")[0];
	LangStringEscape = LangFindDelimiter("string-esc")[0];
	LangOperationEnd = LangFindDelimiter("semicolon")[0];
	LangMacroIndicator = LangFindDelimiter("macro-def")[0];
	LangMacroSetChar = LangFindDelimiter("macro-set")[0];

	//String based:
	dicLang_space = LangFindDelimiter("space");
	dicLang_plus = LangFindDelimiter("plus");
	dicLang_minus = LangFindDelimiter("minus");
	dicLang_multi = LangFindDelimiter("multi");
	dicLang_divide = LangFindDelimiter("divide");
	dicLang_equal = LangFindDelimiter("equal");
	dicLang_pointer = LangFindDelimiter("pointer");
	dicLang_braketOpen = LangFindDelimiter("braketOpen");
	dicLang_braketClose = LangFindDelimiter("braketClose");
	dicLang_bracesOpen = LangFindDelimiter("bracesOpen");
	dicLang_bracesClose = LangFindDelimiter("bracesClose");
	dicLang_power = LangFindDelimiter("power");
	dicLang_exclamation = LangFindDelimiter("exclamation");
	dicLang_greater = LangFindDelimiter("greater");
	dicLang_smaller = LangFindDelimiter("smaller");
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
	dicLangKey_cond_if = LangFindKeyword("cond-if");
	dicLangKey_cond_else = LangFindKeyword("cond-else");
	dicLangKey_cond_break = LangFindKeyword("cond-break");
	dicLangKey_loop_while = LangFindKeyword("loop-while");
	dicLangKey_loop_do = LangFindKeyword("loop-do");
	dicLangKey_loop_break = LangFindKeyword("loop-break");
	dicLangKey_function = LangFindKeyword("function");
	dicLangKey_return = LangFindKeyword("return");
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
/** Check whether a char delimiter is an actual register delimiter:
 * 
 * @param string key key name
 * @return bool
 */
bool Lang::LangIsDelimiter(const std::string& value) {
	if (value[0] == dicLang_space[0] && value == dicLang_space) return true;
	if (value[0] == dicLang_plus[0] && value == dicLang_plus) return true;
	if (value[0] == dicLang_minus[0] && value == dicLang_minus) return true;
	if (value[0] == dicLang_multi[0] && value == dicLang_multi) return true;
	if (value[0] == dicLang_divide[0] && value == dicLang_divide) return true;
	if (value[0] == dicLang_equal[0] && value == dicLang_equal) return true;
	if (value[0] == dicLang_pointer[0] && value == dicLang_pointer) return true;
	if (value[0] == dicLang_braketOpen[0] && value == dicLang_braketOpen) return true;
	if (value[0] == dicLang_braketClose[0] && value == dicLang_braketClose) return true;
	if (value[0] == dicLang_bracesOpen[0] && value == dicLang_bracesOpen) return true;
	if (value[0] == dicLang_bracesClose[0] && value == dicLang_bracesClose) return true;
	if (value[0] == dicLang_power[0] && value == dicLang_power) return true;
	if (value[0] == dicLang_exclamation[0] && value == dicLang_exclamation) return true;
	if (value[0] == dicLang_greater[0] && value == dicLang_greater) return true;
	if (value[0] == dicLang_smaller[0] && value == dicLang_smaller) return true;
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

