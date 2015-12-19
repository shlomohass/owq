/* 
 * File:   Lang.h
 * Author: shlomi
 *
 * Created on 10 דצמבר 2015, 17:15
 */

#ifndef LANG_H
#define	LANG_H


#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

class Lang {
public:
    Lang();
    Lang(const Lang& orig);
    
    //Regex lib:
    static map<string, string>     LangRegexLib;
    
    //Allowed extensions:
    static vector<string>          extensionLib;
    
    //Delimiters Definition and functions:
    static map<string, char>       LangDelimiter;
    static map<char, string>       LangInverseDelimiter;
    static char                    LangFindDelimiter(const string& key);
    static bool                    LangHasKeyDelimiter(const string& key);
    static bool                    LangIsDelimiter(const char& value);
    static bool                    LangIsDelimiter(const string& value);
    
    //Digits Definition and functions:
    static map<int, char>          LangDigits;
    
    
    //Keywords Definition and functions:
    static map<string, string>     LangKeywords;
    static map<string, string>     LangInverseKeywords;
    static string                  LangFindKeyword(const string& key);
    static bool                    LangHasKeyKeyword(const string& key);
    static bool                    LangIsKeyword(const string& value);
    
    //Naming system Definition and functions:
    static vector<char>            LangNamingAllowedChars;
    static bool                    LangIsNamingAllowed(const string& value);
    static bool                    LangIsNamingAllowed(const char& value);
    
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
    static void printHeader(string headername);
private:

};

#endif	/* LANG_H */

