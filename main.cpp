/* 
 * File:   main.cpp
 * Author: shlomi
 */ 

//#include <exception>
//#include <Awesomium/WebCore.h>
//#include <Awesomium/BitmapSurface.h>
//#include <Awesomium/STLHelpers.h>

#define WIDTH   800
#define HEIGHT  600
#define URL     "http://www.google.com"

#include "Setowq.h"
#include "Lang.h"
#include "Script.h"

using namespace std;

int main(int argc, char** argv) {
    
    /*
    Awesomium::WebCore* web_core = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
    Awesomium::WebView* view = web_core->CreateWebView(WIDTH, HEIGHT);
    Awesomium::WebURL url(Awesomium::WSLit(URL));
    view->LoadURL(url);
    while (view->IsLoading())
        web_core->Update();
    Sleep(300);
    web_core->Update();
    // Get the WebView's rendering Surface. The default Surface is of
    // type 'BitmapSurface', we must cast it before we can use it.
    Awesomium::BitmapSurface* surface = (Awesomium::BitmapSurface*)view->surface();

    // Make sure our surface is not NULL-- it may be NULL if the WebView 
    // process has crashed.
    if (surface != 0) {
      // Save our BitmapSurface to a JPEG image in the current
      // working directory.
      surface->SaveToJPEG(Awesomium::WSLit("./result.jpg"));
    }
    view->Destroy();

    Awesomium::WebCore::Shutdown();
     * */
    /*
    Lang* lang = new Lang();
    Script s;

    //Register global system variables to Interpreter:
    //s.registerVariable("Aflag",   RegisteredVariable::REGISTERED_DOUBLE, &Aflag);
    
    //Load target script:
    bool indicator = s.loadFile("src/example.scs", OWQ_DEBUG);
    //Loading success so go and do stuff:
    if (indicator) {
        s.run(OWQ_DEBUG);
    }

    delete lang;
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