/* 
 * File:   Lang.h
 * Author: shlomi
 *
 * Created on 10 דצמבר 2015, 17:15
 */

#ifndef LANG_H
#define	LANG_H


#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

//Important Language Macros:

class Lang {
public:
    Lang();
    Lang(const Lang& orig);
    
    //Allowed extensions:
    static std::vector<std::string> extensionLib;
    
    //Delimiters Definition and functions:
    static std::map<std::string, std::string> LangDelimiter;
    static std::map<std::string, std::string> LangInverseDelimiter;
    static std::string LangFindDelimiter(const std::string& key);
    static bool LangHasKeyDelimiter(const std::string& key);
    static bool LangIsDelimiter(const char& value);
    static bool LangIsDelimiter(const std::string& value);
    static bool LangIsComparison(const char& value);
    static bool LangIsComparison(const std::string& value);
    static bool LangIsOfCondition(const char& value);
    static bool LangIsOfCondition(const std::string& value);
    //Digits Definition and functions:
    static std::map<int, char> LangDigits;
    
    
    //Keywords Definition and functions:
    static std::map<std::string, std::string>     LangKeywords;
    static std::map<std::string, std::string>     LangInverseKeywords;
    static std::string LangFindKeyword(const std::string& key);
    static bool		   LangHasKeyKeyword(const std::string& key);
    static bool		   LangIsKeyword(const std::string& value);
	static void		   LangPopulate();
    //Naming system Definition and functions:
    static std::vector<char> LangNamingAllowedChars;
    static bool              LangIsNamingAllowed(const std::string& value);
    static bool              LangIsNamingAllowed(const char& value);
    
    //Define constant definition characters this is auto set and will insure speed up drasticly:
    
	//Char based:
	static char LangGeneralSpace;
	static char LangStringIndicator;
    static char LangFunctionOpenArguChar;
    static char LangFunctionCloseArguChar;
    static char LangBlockOpenChar;
    static char LangBlockCloseChar;
    static char LangArgumentSpacer;
    static char LangStringEscape;
    static char LangOperationEnd;
    static char LangMacroIndicator;
    static char LangMacroSetChar;

	//String based language symbols and keywords:
	static std::string dicLang_space;
	static std::string dicLang_plus;
	static std::string dicLang_minus;
	static std::string dicLang_multi;
	static std::string dicLang_divide;
	static std::string dicLang_equal;
	static std::string dicLang_braketOpen;
	static std::string dicLang_braketClose;
	static std::string dicLang_bracesOpen;
	static std::string dicLang_bracesClose;
	static std::string dicLang_power;
	static std::string dicLang_exclamation;
	static std::string dicLang_greater;
	static std::string dicLang_smaller;
	static std::string dicLang_comma;
	static std::string dicLang_c_tequal;
	static std::string dicLang_c_ntequal;
	static std::string dicLang_c_equal;
	static std::string dicLang_c_nequal;
	static std::string dicLang_and;
	static std::string dicLang_or;
	static std::string dicLang_semicolon;
	//String based keywords:
	static std::string dicLangKey_variable;
	static std::string dicLangKey_cond_if;
	static std::string dicLangKey_cond_else;
	static std::string dicLangKey_cond_break;
	static std::string dicLangKey_loop_while;
	static std::string dicLangKey_loop_do;
	static std::string dicLangKey_loop_break;
	static std::string dicLangKey_function;
	static std::string dicLangKey_return;

    virtual ~Lang();
    
    //Printing stuff:
    static void printHeader(std::string headername);
    static void printSepLine(int breaks);
    static void printEmpLine(int breaks);

};

#endif	/* LANG_H */

