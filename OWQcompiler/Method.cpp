/* 
 * File:   Method.h
 * Author: shlomo hassid
 *
 */

#include "Method.h"

Method::Method() {
	name = "";
}
Method::Method(int address) : Loop(address) {
    name = "";
}

Method::Method(int address, int xRetAddress, std::string setName) : Loop(address) {
    retAddress = xRetAddress;
    name = setName;
}

int Method::getReturnAddress() {
    return retAddress;
}

std::string* Method::getName() {
    return &name;
}