
#ifndef ARGU_H
#define	ARGU_H

#include <boost\filesystem\operations.hpp>
#include <boost\filesystem\path.hpp>
#include <string>
#include <iostream>

namespace fs = boost::filesystem;

class Argu
{

public:

	fs::wpath inscript;

	Argu();
	virtual ~Argu();
};

#endif // end of ARGU_H
