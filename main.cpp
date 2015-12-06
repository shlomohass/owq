/* 
 * File:   main.cpp
 * Author: shlomi
 */

#include <exception>
#include "Parser.h"


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

/*
 * 
 */
int main(int argc, char** argv) {
    
    Script s;
    
    //Load defaults GLOBAL APPLICATION VARIABLES:
    double Aflag = 3;
    double Atest = 4;
    string Aauthor = "Shlomo Hassid";
    s.registerVariable("Aflag",   RegisteredVariable::REGISTERED_DOUBLE, &Aflag);
    s.registerVariable("Atest",   RegisteredVariable::REGISTERED_DOUBLE, &Atest);
    s.registerVariable("Author",  RegisteredVariable::REGISTERED_STRING, &Aauthor);
    
    s.load("src/example.scs");
    
    
    s.render();
    
    s.run();
    
    //s.run();
    
    //s.execute("print(\"\shlomi\")");
    /*
    double offset = 100;
    s.registerVariable("offset",REGISTERED_DOUBLE,&offset);

    s.execute("equation(60,offset)");
    s.unregisterVariable("offset");
    */
    
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