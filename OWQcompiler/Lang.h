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
    
    //Regex lib:
    static std::map<std::string, std::string> LangRegexLib;
    
    //Allowed extensions:
    static std::vector<std::string> extensionLib;
    
    //Delimiters Definition and functions:
    static std::map<std::string, std::string>     LangDelimiter;
    static std::map<std::string, std::string>     LangInverseDelimiter;
    static std::string                            LangFindDelimiter(const std::string& key);
    static bool                    LangHasKeyDelimiter(const std::string& key);
    static bool                    LangIsDelimiter(const char& value);
    static bool                    LangIsDelimiter(const std::string& value);
    static bool                    LangIsComparison(const char& value);
    static bool                    LangIsComparison(const std::string& value);
    static bool                    LangIsOfCondition(const char& value);
    static bool                    LangIsOfCondition(const std::string& value);
    //Digits Definition and functions:
    static std::map<int, char>          LangDigits;
    
    
    //Keywords Definition and functions:
    static std::map<std::string, std::string>     LangKeywords;
    static std::map<std::string, std::string>     LangInverseKeywords;
    static std::string                  LangFindKeyword(const std::string& key);
    static bool                         LangHasKeyKeyword(const std::string& key);
    static bool                         LangIsKeyword(const std::string& value);
    
    //Naming system Definition and functions:
    static std::vector<char> LangNamingAllowedChars;
    static bool              LangIsNamingAllowed(const std::string& value);
    static bool              LangIsNamingAllowed(const char& value);
    
    //Define constant definition characters:
    static char                    LangStringIndicator;
    static char                    LangFunctionOpenArguChar;
    static char                    LangFunctionCloseArguChar;
    static char                    LangBlockOpenChar;
    static char                    LangBlockCloseChar;
    static char                    LangArgumentSpacer;
    static char                    LangStringEscape;
    static char                    LangOperationEnd;
    static char                    LangMacroIndicator;
    static char                    LangMacroSetChar;
    virtual ~Lang();
    
    //Printing stuff:
    static void printHeader(std::string headername);
    static void printSepLine(int breaks);
    static void printEmpLine(int breaks);

};

#endif	/* LANG_H */

