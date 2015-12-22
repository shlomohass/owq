/* 
 * File:   main.cpp
 * Author: shlomi
 */

#include <exception>
#include "Lang.h"
#include "Script.h"

struct QuitNow: public exception
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

int main(int argc, char** argv) {
    
    Lang* lang = new Lang();
    Script s;
    
    //bool indicator = s.loadFile("src/example.scs");
    //Parser parser;
    
    string testing1 = "ShlomiHassid1";
    string testing2 = "ShlomiHassid2";
    string testing3 = "ShlomiHassid3";
    string testing4 = "ShlomiHassid4";
    
    
    /*
    testing1 = parser.toLowerString(&testing1);
    string testing2 = parser.toUpperString(&testing1);
    
    cout << testing1 << endl;
    cout << testing2 << endl;
    
    
    //Load defaults GLOBAL APPLICATION VARIABLES:
    double Aflag = 3;
    double Atest = 4;
    string Aauthor = "the Author";
    
    //Register global system variables to Interpreter:
    //s.registerVariable("Aflag",   RegisteredVariable::REGISTERED_DOUBLE, &Aflag);
    //s.registerVariable("Atest",   RegisteredVariable::REGISTERED_DOUBLE, &Atest);
    //s.registerVariable("Author",  RegisteredVariable::REGISTERED_STRING, &Aauthor);
    
     */
    //Load target script:
    bool indicator = s.loadFile("src/example.scs", true);
    
    //Loading success so go and do stuff:
    if (indicator) {
        s.render();
        //s.run();
    }
    
    delete lang;
    
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