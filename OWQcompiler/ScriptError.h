/* 
 * File:   ScriptError.h
 * Author: shlomo hassid
 *
 */

#ifndef SCRIPTERROR_H
#define	SCRIPTERROR_H

using namespace std;

#include <string>
#include <iostream>

class ScriptError {
public:
    ScriptError();
    static void msg(string event);
    static void render(string msg);
    virtual ~ScriptError();
};

#endif	/* SCRIPTERROR_H */

