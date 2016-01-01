/*
* File:   main.cpp
* Author: shlomi
*/

#include <exception>

#include "Setowq.h"
#include "Lang.h"
#include "Script.h"

#ifndef OWQ_NAN
#define OWQ_NAN -3231307.6790
#endif


using namespace std;

int main(int argc, char** argv) {

	Lang* lang = new Lang();
	Script s;

	//Register global system variables to Interpreter:
	//s.registerVariable("Aflag",   RegisteredVariable::REGISTERED_DOUBLE, &Aflag);

	//Load target script:
	bool indicator = s.loadFile("assets\\example.scs", OWQ_DEBUG);
	//Loading success so go and do stuff:
	if (indicator) {
	s.run(OWQ_DEBUG);
	}

	delete lang;

	system("pause");
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