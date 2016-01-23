/* 
 * File:   Source.cpp
 * Author: shlomi
 * 
 * Created on 17 דצמבר 2015, 19:32
 */
#include <sstream>
#include "Source.h"
#include "Lang.h"

namespace Eowq {

	Source::Source(char d_str, char str_esc, char d_blo_open, char d_blo_close, char d_op, char d_mIndi, char d_mSet, char d_mOFunc, char d_mCFunc, char d_mSFunc) {
		deli_string = d_str;
		string_escape = str_esc;
		deli_block_open = d_blo_open;
		deli_block_close = d_blo_close;
		deli_op = d_op;
		inverted_deli_op = '\a';
		macroIndicator = d_mIndi;
		macroSetSymbol = d_mSet;
		macroFuncOpen = d_mOFunc;
		macroFuncClose = d_mCFunc;
		macroArguSpacer = d_mSFunc;
		instring = false;
		inMacro = false;
		inMacroFunc = false;
		inMacroNested = 0;
		inMacroSet = false;
		macroSet = "";
		macroArgBuffer = "";
		macroCounter = 0;
		mcomment = false;
		skipToEnd = false;
		hasDelimitter = false;
		bufferSize = 0;
		prevchar = '\0';
		macroArguments.reserve(10);
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
	void Source::pushLine(std::string line, int linenumber) {
		lines.push_back(line);
		if (lineNumbers.size() < 2) {
			lineNumbers.push_back(linenumber);
		}
		else {
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
		}
		else {
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
		}
		else if (mcomment) {
			prevchar = ch;
			return false;
		}

		//Open a macro:
		if (!instring && ch == macroIndicator && !inMacro) {
			inMacro = true;
			inMacroSet = false;
			inMacroFunc = false;
			macroBuffer = "";
			macroArgBuffer = "";
			return false;
		}
		//A makro arguments start here:
		if (inMacro && !inMacroSet && !inMacroFunc && ch == macroFuncOpen) {
			//must have a name
			if (macroBuffer != "" && macros.find(macroBuffer) != macros.end()) {
				inMacroFunc = true;
				macroArgBuffer = "";
				return false;
			}
			else {
				//Return a naming error in macros: macro don't exists or empty
				macroBuffer = "";
				macroArgBuffer = "";
				inMacro = false;
				inMacroFunc = false;
				inMacroNested = 0;
				macroArguments.clear();
				return false;
			}
		}
		//Stop a macro function call:
		if (inMacro && !inMacroSet && inMacroFunc && ch == macroIndicator) {
			inMacro = false;
			inMacroFunc = false;
			macroBuffer = "";
			macroArguments.clear();
			return false;
		}
		//Parse macro arguments until end:
		if (inMacro && !inMacroSet && inMacroFunc) {
			if (ch == macroArguSpacer && macroArgBuffer.length() > 0 && inMacroNested == 0) {
				if (macroArgBuffer.at(macroArgBuffer.length() - 1) != string_escape) {
					macroArguments.push_back(macroArgBuffer);
					macroArgBuffer = "";
				}
				else {
					macroArgBuffer.pop_back();
					macroArgBuffer += ch;
				}
				return false;
			}
			if (ch == macroFuncOpen) {
				inMacroNested++;
			}
			if (ch == macroFuncClose && inMacroNested == 0) {
				if (macroArgBuffer.length() > 0) {
					macroArguments.push_back(macroArgBuffer);
					macroArgBuffer = "";
				}
				if (macroArguments.size() > 0) {
					while (macroArguments.size() > 0) {
						std::string temp_mac = macros[macroBuffer];
						macroFuncAppend(temp_mac);
						buffer += temp_mac;
						bufferSize += (int)temp_mac.length();
					}
					usedMacrosCounter[macroBuffer]++;
					macroCounter++;
					macroBuffer = "";
					macroArgBuffer = "";
					inMacroNested = 0;
					return false;
				}
			}
			else if (ch == macroFuncClose) {
				inMacroNested--;
			}
			macroArgBuffer += ch;
			return false;
		}
		//Close a macro -> regular macro call or definition macro:
		if (inMacro && !inMacroSet && ch == macroIndicator) {
			if (macroBuffer != "" && macros.find(macroBuffer) != macros.end()) {
				buffer += macros[macroBuffer];
				bufferSize += (int)macros[macroBuffer].length();
				usedMacrosCounter[macroBuffer]++;
				macroCounter++;
				macroBuffer = "";
				macroArgBuffer = "";
				inMacro = false;
				inMacroFunc = false;
				macroArguments.clear();
				return false;
			}
			else {
				//Return a naming error in macros: macro don't exists
				macroBuffer = "";
				macroArgBuffer = "";
				inMacro = false;
				inMacroFunc = false;
				macroArguments.clear();
				return false;
			}
		}
		//Set a macro name:
		if (inMacro && !inMacroSet) {
			if (!Lang::LangIsNamingAllowed(ch) && ch != macroSetSymbol) {
				return false;
			}
			if (ch != macroSetSymbol) {
				macroBuffer += ch;
				return false;
			}
			else {
				if (macroBuffer != "" && macros.find(macroBuffer) == macros.end()) {
					macros.insert(std::pair<std::string, std::string>(macroBuffer, ""));
					usedMacrosCounter.insert(std::pair<std::string, int>(macroBuffer, 0));
					macroSet = macroBuffer;
					macroBuffer = "";
					macroArgBuffer = "";
					inMacroSet = true;
					inMacroFunc = false;
					macroArguments.clear();
					return false;
				}
				else {
					//Return a naming error in macros: name empty or exists
					return false;
				}
			}
			return false;
		}
		//Set the definition macro value:
		if (inMacro && inMacroSet) {
			if (ch == macroIndicator) {
				if (macroBuffer.back() != string_escape) {
					if (macroSet.size() > 0 && macros.find(macroSet) != macros.end()) {
						macros[macroSet] = macroBuffer;
						macroSet = "";
						macroArgBuffer = "";
						inMacro = false;
						inMacroSet = false;
						inMacroFunc = false;
						macroArguments.clear();
					}
					else {
						//Error macro set a unknown macro naming
						return false;
					}
				}
				else {
					macroBuffer.pop_back();
					macroBuffer += ch;
				}
			}
			else {
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
		}
		else
			//Indicates an string ending or opening:
			if (ch == deli_string && !hasDelimitter) {
				instring = instring ? false : true;
				buffer += ch;
				prevchar = ch;
				bufferSize++;
			}
			else
				//Indicates a single comment start point:
				if (bufferSize > 0 && ch == '/' && prevchar == '/' && !instring) {
					buffer.pop_back();
					prevchar = '\0';
					bufferSize--;
					skipToEnd = true;
					return false;
				}
				else
					//Indicates a multi comment start point:
					if (bufferSize > 0 && !instring && ch == '*' && prevchar == '/') {
						buffer.pop_back();
						prevchar = '\0';
						bufferSize--;
						mcomment = true;
						return false;
					}
					else
						//Remove garbage characters:
						if (!instring && (ch == '\t' || ch == '\r' || ch == '\b' || ch == '\a')) {
							buffer += ' ';
							prevchar = ' ';
							bufferSize++;
						}
						else
							//Remove the inverted character if its present by the code:
							if (ch == inverted_deli_op) {
								prevchar = ' ';
								//This will make sure our inverted char is not present in the file -> "no alert tab char allowed".
								return false;
							}
							else
								//Detect Block open | close
								if (!instring && (ch == deli_block_open || ch == deli_block_close)) {
									buffer += ch;
									prevchar = ch;
									bufferSize++;
									return true;
								}
								else
									//Detect instruction ending for compiler help:
									if (!instring && ch == deli_op) {
										buffer += inverted_deli_op;
										prevchar = '\0';
										bufferSize++;
										return true;
									}
		//Add character (nothing special but don't save double space):
									else {
										if (ch == ' ' && !instring && prevchar == ' ') {
											return false;
										}
										buffer += ch;
										prevchar = ch;
										bufferSize++;
										return false;
									}
									return false;
	}

	/** Validate if a single buffer line is valid for compilation
	 *
	 * @return boolean
	 */
	bool Source::validateLine() {
		int size = (int)buffer.size();
		for (int j = size - 1; j >= 0; j--) {
			if (buffer[j] == ' ') {
				continue;
			}
			if (
				buffer[j] == deli_block_open
				|| buffer[j] == deli_block_close
				|| buffer[j] == inverted_deli_op
				) {
				return true;
			}
			else {
				return false;
			}
		}
		return false;
	}
	/** Append macro variables to value:
	 *
	 * @param string ref temp_res
	 */
	void Source::macroFuncAppend(std::string& temp_res) {
		bool insert_flag = false;
		int  size = (int)temp_res.length();
		std::string strbuild = "";
		std::vector<int> toearase;
		for (int i = 0; i < size; i++) {
			char c = temp_res[i];
			if (c == '$') {
				if (i > 0 && temp_res[i - 1] == string_escape) {
					continue;
				}
				insert_flag = true;
			}
			else if (insert_flag && '1' <= c && c <= '9') {
				int in = ((int)c - 48) - 1;
				if ((int)macroArguments.size() > in) {
					strbuild += macroArguments[in];
					if (find(toearase.begin(), toearase.end(), in) == toearase.end()) {
						toearase.push_back(in);
					}
				}
				insert_flag = false;
			}
			else {
				strbuild += c;
			}
		}
		std::vector<std::string> leftArgu;
		//Erase from macroArguments:
		for (int i = 0; i < (int)macroArguments.size(); i++) {
			if (find(toearase.begin(), toearase.end(), i) == toearase.end()) {
				leftArgu.push_back(macroArguments[i]);
			}
		}
		macroArguments = leftArgu;
		temp_res = strbuild;
	}

	/** Clean buffer from unneeded chars op_end;
	 *
	 * @return boolean
	 */
	void Source::cleanLine() {
		int bufsize = unsavedSize();
		for (int j = bufsize - 1; j >= 0; j--) {
			if (buffer[j] == inverted_deli_op || buffer[j] == ' ') {
				buffer.erase(j, 1);
				bufferSize--;
			}
			else {
				break;
			}
		}
		bufsize = unsavedSize();
		std::string temp_buf = "";
		char prev_char = '\0';
		bool in_string = false;
		for (int i = 0; i < bufsize; i++) {
			char tch = buffer[i];
			if (!in_string && tch == ' ' && prev_char == ' ') {
				continue;
			}
			else if (!in_string && (tch == '\t' || tch == '\n' || tch == '\r' || tch == '\b')) {
				if (prev_char == ' ')
					continue;
				else
					tch = ' ';
			}
			else if (!in_string && tch == deli_op) {
				temp_buf += tch;
				tch = ' ';
			}
			else if (tch == deli_string && prev_char != string_escape) {
				in_string = in_string ? false : true;
			}
			temp_buf += tch;
			prev_char = tch;
		}
		buffer = temp_buf;
		bufferSize = buffer.length();
	}
	/** Clear the Object or even can be called reset:
	 *
	 * @return
	 */
	void Source::clearLines() {
		lines.clear();
		buffer.clear();
		macroBuffer.clear();
		macroArguments.clear();
		macroArgBuffer.clear();
		inMacro = false;
		inMacroSet = false;
		inMacroFunc = false;
		inMacroNested = 0;
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
	std::string Source::getLines() {
		std::ostringstream output;
		std::vector<std::string>::iterator src_iter;
		for (src_iter = lines.begin(); src_iter != lines.end(); src_iter++) {
			if (!output.str().empty()) {
				output << ' ' << *src_iter;
			}
			else {
				output << *src_iter;
			}
		}
		return output.str();
	}
	/** Return a line of the combined vector line numbers
	 *  @return string
	 */
	std::string Source::getLineNumbers() {
		std::ostringstream output;
		std::vector<int>::iterator src_iter;
		for (src_iter = lineNumbers.begin(); src_iter != lineNumbers.end(); src_iter++) {
			if (!output.str().empty()) {
				output << '-' << *src_iter;
			}
			else {
				output << *src_iter;
			}
		}
		return output.str();
	}
	/** Render functions for debugger source pre parsed
	 */
	void Source::renderSource() {
		std::cout << " * From Original Line Scope --> " << getLineNumbers() << std::endl << std::endl;
		for (int i = 0; i < (int)lines.size(); i++) {
			std::cout << "\t[" << i << "] -> " << lines[i] << std::endl;
		}
		std::cout << std::endl << "   CODE --> " << getLines() << std::endl;
	}
	void Source::renderMacros() {
		std::cout << " * Total registered Macros --> " << macros.size() << std::endl;
		std::cout << " * Total Macros used       --> " << macroCounter << std::endl << std::endl;
		typedef std::map<std::string, std::string>::iterator it_type;
		for (it_type iterator = macros.begin(); iterator != macros.end(); iterator++) {
			std::cout << "\tM[" << iterator->first << "](" << usedMacrosCounter[iterator->first] << ")-> " << iterator->second << std::endl;
		}
		std::cout << std::endl;
	}
}