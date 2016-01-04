
#ifndef ARGU_H
#define	ARGU_H

#include <boost\filesystem.hpp>
#include <boost\filesystem\fstream.hpp>
#include <boost\filesystem\operations.hpp>
#include <boost\filesystem\path.hpp>
#include <string>
#include <iostream>

namespace fs = boost::filesystem;

class Argu
{
public:
	std::string execfile;
	fs::path curdir;
	fs::path testdir;
	fs::path outdir;

	Argu();
	virtual ~Argu();
};

#endif // end of ARGU_H
