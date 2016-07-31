/* 
 * File:   Method.h
 * Author: shlomo hassid
 *
 */

#ifndef OWQLOOP_H
#define	OWQLOOP_H

#include "ScriptVariable.h"
#include <map>
#include <unordered_map>
namespace Eowq
{
	class Script;

	class Loop {

	protected:

		std::unordered_map<std::string, ScriptVariable> var;
		int varSize;
		int personalAddress;

	public:

		//Constructors:
		Loop();
		Loop(int address);

		//Push method variables by types:
		bool addVariable(std::string& name);
		bool addVariable(std::string& name, StackData& sd);

		//Get a scoped method variable:
		std::unordered_map<std::string, ScriptVariable>::iterator getVariableContainerEnd();
		std::unordered_map<std::string, ScriptVariable>::iterator getVariableIt(std::string& name);
		ScriptVariable *getVariable(std::string& name);
		bool hasVariable(std::string& name);


		//Setter getter address:
		void setPersonalAddress(int address);
		int  getPersonalAddress();

		//Deref variables:
		void derefInScope(std::string& name);

		//Delete from scope:
		void deleteFromScope(Script* script, std::unordered_map<std::string, ScriptVariable>::iterator& it);
		void deleteFromScope(Script* script, std::unordered_map<std::string, ScriptVariable>::iterator& it, bool avoidDelete);

		//Reset personal scope:
		void resetScope(Script* script);

		//Render all registered variables:
		void renderScopeVars();

	};
}
#endif	/* OWQLOOP_H */

