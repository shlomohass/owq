/*
* File:   main.cpp
* Author: shlomi
*/

#include <boost\filesystem.hpp>
#include <boost\filesystem\fstream.hpp>
#include <boost\filesystem\operations.hpp>
#include <boost\filesystem\path.hpp>

#include <string>
#include <locale>
#include <iterator>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <memory>
#include <time.h>

#include "argvparser.h" //http://mih.voxindeserto.de/argvparser.html
#include "Argu.h"

namespace fs = boost::filesystem;
namespace cm = CommandLineProcessing;

std::wstring exec(const char* cmd) {
	std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) return L"ERROR";
	wchar_t buffer[128];
	std::wstring result = L"";
	while (!feof(pipe.get())) {
		if (fgetws(buffer, 128, pipe.get()) != NULL)
			result += buffer;
	}
	return result;
}

std::wstring unescape(const std::wstring& str) {
	std::wstring res;
	for (int i = 0; i<(int)str.length(); ++i) {
		switch (str[i]) {
		case '\r':
			res += L"\\r";
			break;
		case '\n':
			res += L"\\n";
			break;
		case '\\':
			res += L"\\\\";
			break;
		case '\t':
			res += L"\\t";
			break;
			//add other special characters if necessary...
		default:
			res += str[i];
		}
	}
	return res;
}

int main(int argc, char** argv) {
	
	//Define args and project settings:
	int exitCode = 0;
	std::string exten = ".towq";

	Argu settings;
	cm::ArgvParser cmd;
	cmd.addErrorCode(0, "Success");
	cmd.addErrorCode(1, "Error");
	cmd.setIntroductoryDescription("This is foo written by bar.");
	cmd.setHelpOption("h", "help", "Print this help page");
	cmd.defineOption("owq", "The owq executable path or file name", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("in", "The test folder path or folder name", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("out", "The result folder to write too", cm::ArgvParser::OptionRequiresValue);

	int result = cmd.parse(argc, argv);


	//Parse and set Argu:
	if (result != cm::ArgvParser::NoParserError)
	{
		std::cout << cmd.parseErrorDescription(result);
		if (result == cm::ArgvParser::ParserHelpRequested) {
			exitCode = 0;
		}
		exitCode = 1;
	}
	else {
		if (cmd.foundOption("owq")) {
			settings.execfile = cmd.optionValue("owq");
		}
		if (cmd.foundOption("in")) {
			settings.testdir = fs::path(cmd.optionValue("in"));
		}
		if (cmd.foundOption("out")) {
			settings.outdir = fs::path(cmd.optionValue("out"));
		}
	}
	
	if (result == cm::ArgvParser::ParserHelpRequested) {
		exit(exitCode);
	}

	//Check if executable is here:

	std::vector<fs::wpath> test_files;
	std::map <std::wstring, std::wstring> expected_results_list;
	std::map <std::wstring, std::wstring> computed_results_list;

	int testcount = 0;

	//Check if tests folder is here and scan the directory:
	if (fs::exists(settings.testdir) || fs::exists(settings.outdir))
	{
		std::wcout << L"\n * OWQexecutiontest -> Test IN and OUT Folders Found; " << std::endl << std::endl;
		
		//Get list of tests:
		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_itr(settings.testdir); dir_itr != end_iter; ++dir_itr)
		{
			try
			{
				if (fs::is_regular_file(dir_itr->status()) && dir_itr->path().extension() == exten)
				{
					++testcount;
					test_files.push_back(dir_itr->path());
				}
			} catch (const std::exception & ex) {
				std::wcout << std::endl << L" * ERROR: When listing test directory: " <<  dir_itr->path().filename() << L" " << ex.what() << std::endl;
			}
		}
		//Make sure we have tests:
		if (testcount > 0) {
			//Parse all tests:
			for (int i = 0; i < (int)test_files.size(); i++) {
				std::wifstream wifs;
				std::wstring txtline;
				int c = 0;
				wifs.open(test_files[i].wstring());
				if (!wifs.is_open())
				{
					std::wcerr << L"Unable to open file: " << test_files[i].wstring() << std::endl;
					expected_results_list[test_files[i].wstring()] = L"@SKIP@";
					computed_results_list[test_files[i].wstring()] = L"@SKIP@";
					continue;
				}
				bool expectedFlag = false;
				std::wstring expectedBuffer = L"";
				while (getline(wifs, txtline)) {
					if (txtline == L"@expected-output") {
						expectedFlag = true;
						continue;
					}
					if (txtline == L"@end-expected-output") {
						break;
					}
					if (expectedFlag) {
						if (expectedBuffer == L"") {
							expectedBuffer += txtline;
						} else {
							expectedBuffer += L"\n" + txtline;
						}
					}
				}
				//Store results
				if (expectedFlag) {
					expected_results_list[test_files[i].wstring()] = expectedBuffer;
					computed_results_list[test_files[i].wstring()] = L"";
				} else {
					expected_results_list[test_files[i].wstring()] = L"@SKIP@";
					computed_results_list[test_files[i].wstring()] = L"@SKIP@";
				}
			}
		} else {
			std::wcout << std::endl << L" * NOTICE: No tests Found." << std::endl;
		}

		//Execute tests and save results:
		typedef std::map<std::wstring, std::wstring>::iterator it_type;
		for (it_type iterator = expected_results_list.begin(); iterator != expected_results_list.end(); iterator++) {
			std::wstring unescaped = unescape(iterator->second);
			if (unescaped.length() > 48) {
				std::wcout << L" File: " << iterator->first << L" - Expected: " << unescaped.substr(0, 45) << L"...  -  Length[ " << iterator->second.length() << L" ]" << std::endl;
			} else {
				std::wcout << L" File: " << iterator->first << L" - Expected: " << unescaped << L"  -  Length[ " << iterator->second.length() << L" ]" << std::endl;
			}
			if (iterator->second != L"@SKIP@") {
				clock_t tStart = clock();
					std::wstring resultbuf = exec((settings.execfile + " -r \"" + std::string(iterator->first.begin(), iterator->first.end()) + "\"").c_str());
					std::wstring un_escaped_resultbuf = unescape(resultbuf);
					if (un_escaped_resultbuf.length() > 63) {
						std::wcout << L"       Result: " << ((resultbuf == iterator->second) ? L"Test PASSED!" : (L"Test FAILLED!  actual result ----> " + un_escaped_resultbuf.substr(0, 60) + L"...")) << std::endl;
					} else {
						std::wcout << L"       Result: " << ((resultbuf == iterator->second) ? L"Test PASSED!" : (L"Test FAILLED!  actual result ----> " + un_escaped_resultbuf)) << std::endl;
					}
					computed_results_list[iterator->first] = resultbuf;
				clock_t tEnd = clock();
				printf("       Execution: %.2fs, %dms\n\n", (double)(tEnd - tStart) / CLOCKS_PER_SEC, (tEnd - tStart) / (CLOCKS_PER_SEC / 1000));
			} else {
				std::wcout << std::endl;
			}
		}
	} else {
		std::wcout << std::endl << L" * ERROR: Tests or Result folder not found!" << std::endl;
		exitCode = 1;
	}

	//Write to log and output to console:
	
	std::wcout << std::endl;
	system("pause");
	return exitCode;
}