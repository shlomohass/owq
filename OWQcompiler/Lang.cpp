/* 
 * File:   Lang.cpp
 * Author: shlomi
 * 
 * Created on 10 דצמבר 2015, 17:15
 */

#include "Lang.h"

//Initialize maps:
map<string, string> Lang::LangInverseDelimiter = map<string, string>{};
map<string, string> Lang::LangInverseKeywords = map<string, string>{};

//Set Delimiters:
map<string, string> Lang::LangDelimiter = {
    {"space"             , " "},
    {"plus-cast-str"     , "~"},
    {"plus"              , "+"},
    {"minus"             , "-"},
    {"multi"             , "*"},
    {"divide"            , "/"},
    {"equal"             , "="},
    {"braketOpen"        , "("},
    {"braketClose"       , ")"},
    {"bracesOpen"        , "{"},
    {"bracesClose"       , "}"},
    {"power"             , "^"},
    {"exclamation"       , "!"},
    {"greater"           , ">"},
    {"smaller"           , "<"},
    {"comma"             , ","},
    {"c-equal"           , "=="},
    {"and"               , "&"},
    {"or"                , "|"},
    {"semicolon"         , ";"}
};

//Set Keywords:
map<string, string> Lang::LangKeywords = {
    {"variable"     , "let"},
    {"cond-if"      , "if"},
    {"cond-else"    , "else"},
    {"loop-while"   , "while"},
    {"loop-do"      , "do"},
	{"loop-break"   , "break" },
    {"return"       , "return"},
};

//Set chars allowed as names:
//if we do not define '"' as a character, no tokens will be generated for string quoetation marks
vector<char> Lang::LangNamingAllowedChars = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
    '_' 
};

//Set allowed extensions:
vector<string> Lang::extensionLib = {
    "scs",
    "SCS",
    "owq",
    "OWQ"
};

//Define constants definition characters:
char Lang::LangStringIndicator          = '"';
char Lang::LangFunctionOpenArguChar     = '(';
char Lang::LangFunctionCloseArguChar    = ')';
char Lang::LangBlockOpenChar            = '{';
char Lang::LangBlockCloseChar           = '}';
char Lang::LangArgumentSpacer           = ',';
char Lang::LangStringEscape             = '\\';
char Lang::LangOperationEnd             = ';';
char Lang::LangMacroIndicator           = '#';
char Lang::LangMacroSetChar             = ':';

//Set Regex lib:
map<string, string> Lang::LangRegexLib = {
    {"remove-comments"     , "(/\\*([^*]|[\\r\\n]|(\\*+([^*/]|[\\r\\n])))*\\*+/)|(//.*)"}
};   

Lang::Lang() {
    //Inverse Delimiter:    
    for(auto const &ent1 : LangDelimiter) {
        LangInverseDelimiter.insert(pair<string, string>(ent1.second, ent1.first));
    } 
    //Inverse Keywords:    
    for(auto const &ent2 : LangKeywords) {
        LangInverseKeywords.insert(pair<string, string>(ent2.second, ent2.first));
    } 
}
Lang::Lang(const Lang& orig) {
    
}
Lang::~Lang() {
}
/** Find the character that represent a delimiter name:
 * 
 * @param string key
 * @return char
 */
string Lang::LangFindDelimiter(const string& key) {
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
bool Lang::LangHasKeyDelimiter(const string& key) {
    return LangDelimiter.count(key) == 1;
}
/** Check whether a char delimiter is an actual register delimiter:
 * 
 * @param string key key name
 * @return bool
 */
bool Lang::LangIsDelimiter(const string& value) {
    return LangInverseDelimiter.count(value) == 1;
}
bool Lang::LangIsDelimiter(const char& value) {
    return LangInverseDelimiter.count(string(1, value)) == 1;
}
/** Checks if a char|string is a delimiter of comparison:
 *  
 * @param char|string value
 * @return boolean
 *  
 */
bool Lang::LangIsComparison(const char& value) {
    return LangIsComparison(string(1, value));
}
bool Lang::LangIsComparison(const string& value) {
    if (value == LangFindDelimiter("greater")) {
        return true;
    } else if ( value == LangFindDelimiter("smaller")) {
        return true;
    } else if ( value == LangFindDelimiter("c-equal")) {
        return true;
    }
    return false;
}
/** Checks if a char|string is a delimiter of a condition such as AND OR etc:
 *  
 * @param char|string value
 * @return boolean
 *  
 */
bool Lang::LangIsOfCondition(const char& value) {
    return LangIsOfCondition(string(1, value));
}
bool Lang::LangIsOfCondition(const string& value) {
    if (value == LangFindDelimiter("and")) {
        return true;
    } else if ( value == LangFindDelimiter("or")) {
        return true;
    }
    return false;
}
/** Find the string that represent a keyword name (key):
 * 
 * @param string key
 * @return string
 */
string Lang::LangFindKeyword(const string& key) {
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
bool Lang::LangHasKeyKeyword(const string& key) {
    return LangKeywords.count(key) == 1;
}
/** Check whether a string is an actual register keyword:
 * 
 * @param string key key name
 * @return bool
 */
bool Lang::LangIsKeyword(const string& value) {
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
bool Lang::LangIsNamingAllowed(const string& value) {
    if (find(LangNamingAllowedChars.begin(), LangNamingAllowedChars.end(), value[0]) != LangNamingAllowedChars.end()) {
        return true;
    }
    return false;
}


/*
 * PRINTING STUFF:
 */
void Lang::printHeader(string headername) {
    cout << endl << "-------------------------------------------------------------------" << endl;
    cout << "| OWQ Debugger -> " << headername << endl;
    printSepLine(2);
}
void Lang::printSepLine(int breaks) {
    cout << "-------------------------------------------------------------------";
    for (int i = 0; i < breaks; i++) {
        cout << endl;
    }
}
void Lang::printEmpLine(int breaks) {
    for (int i = 0; i < breaks; i++) {
        cout << endl;
    }
}

