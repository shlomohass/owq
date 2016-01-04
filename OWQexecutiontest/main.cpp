/*
* File:   main.cpp
* Author: shlomi
*/

#include <boost\filesystem.hpp>
#include <boost\filesystem\fstream.hpp>
#include <boost\filesystem\operations.hpp>
#include <boost\filesystem\path.hpp>
#include <string>
#include <iterator>
#include <iostream>
#include <vector>
#include <cstdio>
#include <memory>




#include "argvparser.h" //http://mih.voxindeserto.de/argvparser.html
#include "Argu.h"

namespace fs = boost::filesystem;
namespace cm = CommandLineProcessing;

std::string exec(const char* cmd) {
	std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) return "ERROR";
	char buffer[128];
	std::string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			result += buffer;
	}
	return result;
}


int main(int argc, char** argv) {
	
	int exitCode = 0;
	Argu settings;
	cm::ArgvParser cmd;
	cmd.addErrorCode(0, "Success");
	cmd.addErrorCode(1, "Error");
	cmd.setIntroductoryDescription("This is foo written by bar.");
	cmd.setHelpOption("h", "help", "Print this help page");
	cmd.defineOption("owq", "The owq executable path or file name", cm::ArgvParser::OptionRequiresValue);
	int result = cmd.parse(argc, argv);

	if (result != cm::ArgvParser::NoParserError)
	{
		std::cout << cmd.parseErrorDescription(result);
		exitCode = 1;
	}
	else {
		if (cmd.foundOption("owq")) {
			settings.execfile = cmd.optionValue("owq");
		}
	}
	
	std::cout << settings.curdir << std::endl;
	std::cout << settings.execfile << std::endl;

	std::vector<fs::directory_entry> operation_dir;

	//Check if executable is here:

	//Check if tests folder is here and scan the directory:

	//For each file open and search for the test comment Expected result:

	//Execute file and get the output:

	//Compare results:

	//Write to log and output to console:

	system("pause");
	return exitCode;
}