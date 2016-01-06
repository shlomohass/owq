/* 
 * File:   Source.h
 * Author: shlomi
 *
 * Created on 17 דצמבר 2015, 19:32
 */

#ifndef SOURCE_H
#define	SOURCE_H

#include <iostream>
#include <vector>
#include <map>


using namespace std;

class Source {
    
    //Holders
    vector<string> lines;        //Will hold lines to be merged
    string buffer;               //A buffer before setting a line
    char prevchar;               //Previous Char buffer
    int bufferSize;              //Keeps track of buffer size -> number of characters, for avoiding .size() all the time
    vector<int> lineNumbers;     //
    map<string, string> macros;
    map<string, int> usedMacrosCounter;
    int macroCounter;
    string macroBuffer;
	string macroArgBuffer;
	vector<string> macroArguments;
    
    //Definitions:
    char deli_string;       // the open and close character for strings
    char string_escape;     // the escape character for strings
    char deli_block_open;   // Block of code open -> mostly '{'
    char deli_block_close;  // Block of code open -> mostly '}'
    char deli_op;           // Operation close character
    char inverted_deli_op;  // A special character to replace the deli_op -> for later cleaning since our compiler does not handle that
    char macroIndicator;
    char macroSetSymbol;
	char macroFuncOpen;
	char macroFuncClose;
	char macroArguSpacer;

    //Flags
    bool instring;      // flag to know when we are inside a string
    bool inMacro;
    bool inMacroSet;
	bool inMacroFunc;

    string macroSet;
    bool mcomment;      // for multi-line comments
    bool skipToEnd;     // for single line comments
    bool hasDelimitter; // Is the character escaped?
    
public:
    Source(char d_str, char str_esc, char d_blo_open, char d_blo_close, char d_op, char d_mIndi, char d_mSet, char d_mOFunc, char d_mCFunc, char d_mSFunc);
    
    void pushLine(string line, int linenumber);
    void pushLine(int linenumber);
    bool pushChar(char ch);
    bool validateLine();
    void cleanLine();
    void clearLines();
    int unsavedSize();
    void renderSource();
    void renderMacros();
    string getLines();
    string getLineNumbers();

	void macroFuncAppend(string& temp_res);
    
    Source(const Source& orig);
    virtual ~Source();
    
private:

};

#endif	/* SOURCE_H */

