/*
* File:   main.cpp
* Author: shlomi
*/

#include <exception>

#include <boost\filesystem\operations.hpp>
#include <boost\filesystem\path.hpp>

#include "argvparser.h"
#include "Argu.h"
#include "Setowq.h"
#include "Lang.h"
#include "Script.h"

#ifndef OWQ_NAN
#define OWQ_NAN -3231307.6790
#endif

namespace fs = boost::filesystem;
namespace cm = CommandLineProcessing;

int main(int argc, char** argv) {

	//Define args and project settings:
	int exitCode = 0;
	std::string filepath_temp;
	Argu settings;
	cm::ArgvParser cmd;
	bool enable_debug = OWQ_DEBUG;
	bool execute_is_requested = false;

	cmd.addErrorCode(0, "Success");
	cmd.addErrorCode(1, "Error");
	cmd.setIntroductoryDescription("This is foo written by bar.");
	cmd.setHelpOption("h", "help", "Print this help page");
	cmd.defineOption("r", "Compile and run a file -r path.owq", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("debug", "enable deep debug", cm::ArgvParser::NoOptionAttribute);

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
		if (cmd.foundOption("r")) {
			filepath_temp = cmd.optionValue("r");
			settings.inscript = fs::wpath(filepath_temp);
			execute_is_requested = true;
		}
		if (cmd.foundOption("debug")) {
			enable_debug = true;
		}
	}

	if (result == cm::ArgvParser::ParserHelpRequested) {
		system("pause");
		exit(exitCode);
	}
	
	if (execute_is_requested) {
		Eowq::Lang* lang = new Eowq::Lang();
		Eowq::Script s;

		//Register global system variables to Interpreter:
		//s.registerVariable("Aflag",   RegisteredVariable::REGISTERED_DOUBLE, &Aflag);

		//Load target script:
		bool indicator = s.loadFile( settings.inscript, enable_debug);
		//Loading success so go and do stuff:
		if (indicator) {
			//s.run( enable_debug );
		}

		delete lang;
	}

	if (enable_debug) {
		system("pause");
	}
	return 0;
}


//IMPLEMENT THE EXPECTION
/*
#include <exception>  // or #include <stdexcept>
#include <iostream>

struct QuitNow: public std::exception
{
QuitNow() { }
virtual ~QuitNow() throw() { }
virtual const char* what() throw()
{
return "QuitNow: request normal termination of program.\n"
"(You should not see this message. Please report it if you do.)";
}
};

using namespace std;

void loopy( unsigned count )
{
if (count < 20)
{
cout << "count = " << count << endl;
loopy( count +1 );
}
else
{
cout << "Throwing QuitNow.\n";
throw QuitNow();
}
}

int main()
{
try
{
cout << "Starting.\n";
loopy( 0 );
cout << "You shouldn't see this.\n";
}
catch (QuitNow)
{
cout << "Caught it!\n";
}
catch (...)
{
cout << "Something went wrong...\n";
}
cout << "Good-bye.\n";
return 0;
}
*/