/* 
 * File:   ScriptVariable.h
 * Author: shlomo hassid
 *
 */

#include "ScriptVariable.h"

ScriptVariable::ScriptVariable() {
    svalue          = "NaN";
    dvalue          = OWQ_NAN;
    name            = ".invalid.null.initialize.";
    isRegistered    = false;
    address         = NULL;
    type            = RegisteredVariable::GLOBAL_FLEX;
}

ScriptVariable::ScriptVariable(string xName, double value) {
    svalue       = "NaN";
    dvalue       = value;
    name         = xName;
    isRegistered = false;
    address      = NULL;
    type         = RegisteredVariable::GLOBAL_FLEX;
}

ScriptVariable::ScriptVariable(string xName, string value) {
    svalue       = value;
    dvalue       = OWQ_NAN;
    name         = xName;
    isRegistered = false;
    address      = NULL;
    type         = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(string xName, bool reg) {
    svalue       = "NaN";
    dvalue       = OWQ_NAN;
    name         = xName;
    isRegistered = reg;
    address      = NULL;
    type         = RegisteredVariable::GLOBAL_FLEX;
}
ScriptVariable::ScriptVariable(string xName, RegisteredVariable xType, void* xAddress) {
    svalue       = "NaN";
    dvalue       = OWQ_NAN;
    name         = xName;
    isRegistered = true;
    address      = xAddress;
    type         = xType;
}

ScriptVariable::ScriptVariable(string xName) {
    svalue       = "NaN";
    dvalue       = OWQ_NAN;
    name         = xName;
    isRegistered = false;
    address      = NULL;
    type         = RegisteredVariable::GLOBAL_FLEX;
}

string ScriptVariable::getName() {
    return name;
}

string ScriptVariable::getStringValue() {
    if (!isRegistered || ( isRegistered && type == RegisteredVariable::GLOBAL_FLEX)) {
        return svalue;
    } else {
        if (type != RegisteredVariable::REGISTERED_STRING) {
            ScriptError::msg("ScriptVariable is registered but is not of type string, var symbol - " + name);
            return "NaN";
        } else {
            string v = *(static_cast<string*>(address));
            return v;
        }
    }
}

double ScriptVariable::getNumberValue() {
    if (!isRegistered ||(isRegistered && type == RegisteredVariable::GLOBAL_FLEX)) {
        return dvalue;
    } else {
        if (type != RegisteredVariable::REGISTERED_DOUBLE) {
            ScriptError::msg("ScriptVariable is registered but is not of type double, var symbol -" + name);
        } else {
            double v = *(static_cast<double*>(address));
            return v;
        }
    }
    return 0;
}

bool ScriptVariable::isString() {
    if(!isRegistered || (isRegistered && type == RegisteredVariable::GLOBAL_FLEX)) {
        if (svalue != "NaN") {
            return true;
        } else {
            return false;
        }
    } else {
        if (type == RegisteredVariable::REGISTERED_STRING) {
            return true;
        } else {
            return false;
        }
    }
}

bool ScriptVariable::isNumber() {
    if (!isRegistered || (isRegistered && type == RegisteredVariable::GLOBAL_FLEX)) {
        if (dvalue != OWQ_NAN) {
            return true;
        } else {
            return false;
        }
    } else {
        if (type == RegisteredVariable::REGISTERED_DOUBLE) {
            return true;
        } else {
            return false;
        }
    }
}

void ScriptVariable::setNumberValue(double xValue) {
    svalue = "NAN";
    if (!isRegistered || (isRegistered && type == RegisteredVariable::GLOBAL_FLEX)) {
        dvalue = xValue;
    } else {
        if (type != RegisteredVariable::REGISTERED_DOUBLE) {
            ScriptError::msg("script variable is registered but can only accept double value");
        } else {
            double* v = static_cast<double*>(address);
            *v = xValue;
        }
    }
}

void ScriptVariable::setStringValue(string xValue) {
    dvalue = OWQ_NAN;
    if (!isRegistered || (isRegistered && type == RegisteredVariable::GLOBAL_FLEX)) {
        svalue = xValue;
    } else {
        if (type != RegisteredVariable::REGISTERED_STRING) {
            ScriptError::msg("script variable is registered but can only accept double value");
        } else {
            string* s = static_cast<string*>(address);
            *s = xValue;
        }
    }
}

string ScriptVariable::renderVariable() {
    ostringstream strs;
    if (isRegistered && type != RegisteredVariable::GLOBAL_FLEX) {
        if (type != RegisteredVariable::REGISTERED_STRING) {
            strs << "GName -> " << name << "\tSval -> " << getStringValue();
        } else {
            strs << "GName -> " << name << "\tDval -> " << getNumberValue();
        }
    } else {
        strs << "VName -> " << name << "\tSval -> " << getStringValue() << "\tDval -> " << getNumberValue();
    }
    return strs.str();
}
void ScriptVariable::setFromStackData(StackData& sd) {
    if (sd.isNumber()) {
        this->setNumberValue(sd.getNumber());
    } else {
        this->setStringValue(sd.getString());
    }
}

ScriptVariable::~ScriptVariable() {

}
