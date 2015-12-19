/* 
 * File:   Source.cpp
 * Author: shlomi
 * 
 * Created on 17 דצמבר 2015, 19:32
 */

#include "Source.h"
#include "Lang.h"

Source::Source(char d_str, char str_esc, char d_blo_open, char d_blo_close, char d_op, char d_mIndi, char d_mSet) {
    deli_string     = d_str;
    string_escape   = str_esc;
    deli_block_open = d_blo_open;
    deli_block_close = d_blo_close;
    deli_op          = d_op;
    inverted_deli_op = '\a';
    macroIndicator  = d_mIndi;
    macroSetSymbol  = d_mSet;
    instring        = false;
    inMacro         = false;
    inMacroSet      = false;
    macroSet        = "";
    macroCounter    = 0;
    mcomment        = false;
    skipToEnd       = false;
    hasDelimitter   = false;
    bufferSize      = 0;
    prevchar        = '\0';
}
Source::Source(const Source& orig) {
}
Source::~Source() {
}

/** Push a specific line: 
 * 
 * @param string line
 * @param integer linenumber
 */
void Source::pushLine(string line, int linenumber) {
    lines.push_back(line);
    if (lineNumbers.size() < 2) {
        lineNumbers.push_back(linenumber);
    } else {
        lineNumbers.back() = linenumber;
    }
}
/** Push a line: 
 * 
 * @param integer linenumber
 */
void Source::pushLine(int linenumber) {
    if (unsavedSize() < 1) {
        skipToEnd = false;
        return;
    }
    lines.push_back(buffer);
    buffer = "";
    bufferSize = 0;
    prevchar = '\0';
    skipToEnd = false; // This is a flag that indicates a comment so terminate it since this is the end of the line
    if (lineNumbers.size() < 2) {
        lineNumbers.push_back(linenumber);
    } else {
        lineNumbers.back() = linenumber;
    }
}
/** Push char 
 * 
 * @param ch
 * @return boolean
 */
bool Source::pushChar(char ch) {
    
    //A single comment skip:
    if (skipToEnd) {
        return false;
    }
    
    //handle escaped characters:
    if (hasDelimitter && !inMacro) {
        buffer += ch;
        prevchar = '\0';
        bufferSize++;
        hasDelimitter = false;
        return false;
    }
    
    //Handle multi-line comments - will escape characters until closing sequence: 
    if (mcomment && ch == '/' && prevchar == '*') {
        mcomment = false;
        prevchar = '\0';
        return false;
    } else if (mcomment) {
        prevchar = ch;
        return false;
    } 
    
    //Open a macro
    if (!instring && ch == macroIndicator && !inMacro) {
        inMacro = true;
        inMacroSet = false;
        macroBuffer = "";
        return false;
    }
    if (inMacro && !inMacroSet && ch == macroIndicator) {
        if (macroBuffer != "" && macros.find(macroBuffer) != macros.end()) {
            buffer += macros[macroBuffer];
            bufferSize += (int)macros[macroBuffer].size();
            usedMacrosCounter[macroBuffer]++;
            macroCounter++;
            macroBuffer = "";
            inMacro = false;
            return true;
        } else {
            //Return a naming error in macros: macro don't exists
            return false;
        }
    }
    if (inMacro && !inMacroSet) {
        if (!Lang::LangIsNamingAllowed(ch) && ch != macroSetSymbol) {
            return false;
        }
        if (ch != macroSetSymbol) {
                macroBuffer += ch; 
            return false;
        } else {
            if (macroBuffer != "" && macros.find(macroBuffer) == macros.end()) {
                macros.insert(pair<string, string>(macroBuffer,""));
                usedMacrosCounter.insert(pair<string, int>(macroBuffer,0));
                macroSet = macroBuffer;
                macroBuffer = "";
                inMacroSet = true;
                return false;
            } else {
                //Return a naming error in macros: name empty or exists
              return false;
            }
        }
        return false;
    }
    if (inMacro && inMacroSet) {
        if (ch == macroIndicator) {
            if (macroBuffer.back() != string_escape) {
                if (macroSet.size() > 0 && macros.find(macroSet) != macros.end()) {
                    macros[macroSet] = macroBuffer;
                    macroSet = "";
                    inMacro = false;
                    inMacroSet = false;
                } else {
                  //Error macro set a unknown macro naming
                   return false; 
                }
            } else {
                macroBuffer.pop_back();
                macroBuffer += ch;
            }
        } else {
            macroBuffer += ch;
        }
        return false;
    }
    
    //Indicates an escape character is coming:
    if (ch == string_escape && instring) {
        hasDelimitter = hasDelimitter ? false : true;
        buffer += ch;
        prevchar = ch;
        bufferSize++;
        return false;
    } else
    //Indicates an string ending or opening:
    if (ch == deli_string && !hasDelimitter) {
        instring = instring ? false : true;
        buffer += ch;
        prevchar = ch;
        bufferSize++;
    } else
    //Indicates a single comment start point:
    if (bufferSize > 0 && ch == '/' && prevchar == '/' && !instring ) {
        buffer.pop_back();
        prevchar = '\0';
        bufferSize--;
        skipToEnd = true;
        return false;
    } else
    //Indicates a multi comment start point:
    if (bufferSize > 0 && !instring && ch == '*' && prevchar == '/' ) {
        buffer.pop_back();
        prevchar = '\0';
        bufferSize--; 
        mcomment = true;
        return false;
    } else
    //Remove garbage characters:
    if(!instring && (ch == '\t' || ch == '\r' || ch == '\b' || ch == '\a')) {
        buffer += ' ';
        prevchar = ' ';
        bufferSize++; 
    } else
    //Remove the inverted character if its present by the code:
    if (ch == inverted_deli_op) { 
        prevchar = ' ';
        //This will make sure our inverted char is not present in the file -> "no alert tab char allowed".
        return false;
    } else
    //Detect Block open | close
    if (!instring && (ch == deli_block_open || ch == deli_block_close)) {
        prevchar = ch;
        return true;
    } else
    //Detect instruction ending for compiler help:
    if (!instring && ch == deli_op) {
        buffer += inverted_deli_op;
        prevchar = '\0';
        bufferSize++;
        return true;
    } else {
    //Add character (nothing special but don't save double space):
        if (ch == ' ' && !instring && prevchar == ' ') {
            return false;
        }
        buffer += ch;
        prevchar = ch;
        bufferSize++;
        return false;
    }
}

/** Validate if a single buffer line is valid for compilation
 * 
 * @return boolean
 */
bool Source::validateLine() {
    int size = (int)buffer.size();
    for (int j = size-1; j >= 0; j--) {
        if (buffer[j] == ' ') {
            continue;
        }
        if (
                buffer[j] == deli_block_open 
                || buffer[j] == deli_block_close 
                || buffer[j] == inverted_deli_op
        ) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}
/** Clean buffer from unneeded chars op_end;
 * 
 * @return boolean
 */
void Source::cleanLine() {
    int bufsize = unsavedSize();
    for (int j = bufsize-1; j >= 0; j--) {
        if (buffer[j] == inverted_deli_op || buffer[j] == ' ') {
            buffer.erase(j, 1);
            bufferSize--;
        } else {
            break;
        }
    }
    bufsize = unsavedSize();
    int upto = 0;
    for (int i = 0; i < bufsize; i++) {
        if (buffer[i] == ' ') {
            upto++;
        } else {
            break;
        }
    }
    if (upto > 0) {
        buffer.erase(0, upto);
        bufferSize -=upto;
    }
}
/** Clear the Object or even can be called reset:
 *  
 * @return
 */
void Source::clearLines() {
    lines.clear();
    buffer.clear();
    prevchar = '\0';
    bufferSize = 0;
    lineNumbers.clear();
    instring = false;
    mcomment = false;
    skipToEnd = false;
    hasDelimitter = false;
}
/** Get the buffer size:
 * 
 * @return int
 */
int Source::unsavedSize() {
    return (int)buffer.size();
}
/** Return a line of the combined vector lines
 *  @return string
 */
string Source::getLines() {
    ostringstream output;
    vector<string>::iterator src_iter;
    for (src_iter = lines.begin(); src_iter != lines.end(); src_iter++) {
        if (!output.str().empty()) {
            output << ' ' << *src_iter;
        } else {
            output << *src_iter;
        }
    }
    return output.str();
}
/** Return a line of the combined vector line numbers
 *  @return string
 */
string Source::getLineNumbers() {
    ostringstream output;
    vector<int>::iterator src_iter;
    for (src_iter = lineNumbers.begin(); src_iter != lineNumbers.end(); src_iter++) {
        if (!output.str().empty()) {
            output << '-' << *src_iter;
        } else {
            output << *src_iter;
        }
    }
    return output.str();
}
/** Render a debugger source pre parsed
 */
void Source::renderSource() {
    cout << " * From Original Line scope --> " << getLineNumbers() << endl << endl;
    for (int i=0; i < lines.size(); i++) {
        cout << "\t[" << i << "] -> " << lines[i] << endl;
    }
    cout << endl << "    CODE --> " << getLines() << endl;
}
void Source::renderMacros() {
    cout << " * Total registered Macros --> " << macros.size() << endl;
    cout << " * Total Macros used       --> " << macroCounter << endl << endl;
    typedef map<string,string>::iterator it_type;
    for(it_type iterator = macros.begin(); iterator != macros.end(); iterator++) {
        cout << "\tM[" << iterator->first << "](" << usedMacrosCounter[iterator->first] << ")-> " << iterator->second << endl;
    }
    cout << endl;
}