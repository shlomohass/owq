#include "Argu.h"

Argu::Argu()
{
	execfile = "OWQ"; // Assumes its included in the path
	curdir = fs::current_path();
	testdir = fs::path("Tests");
	outdir = fs::path("OwqTestResults");
}


Argu::~Argu()
{

}
