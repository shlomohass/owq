#include "Argu.h"

Argu::Argu()
{
	execfile = "OWQ.exe";
	curdir = fs::current_path();
	testdir = fs::path("OwqTests");
	outdir = fs::path("OwqTestResults");
}


Argu::~Argu()
{
}
