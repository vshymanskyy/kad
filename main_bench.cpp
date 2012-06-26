#include "XTimeCounter.h"
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorFormatter.h>

int main(int argc, char **argv)
{
	CxxTest::ErrorFormatter tmp(new CxxTest::OutputStream(), "", "" );
	if (argc < 2) {
		printf("Please specify the test suite:\n");
		const char* arg[] = { argv[0], "--help-tests" };
		return CxxTest::Main<CxxTest::ErrorFormatter>( tmp, 2, (char**)arg );
	}

	XTimeCounter t;
	for (int i=0; i < 1000; i++) {
		CxxTest::Main<CxxTest::ErrorFormatter>( tmp, argc, argv );
	}
	printf("Test finished in %d ms\n", t.Elapsed());
	return 0;
}
