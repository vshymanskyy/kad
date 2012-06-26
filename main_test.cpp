#include <XTimeCounter.h>
#include <XHelpers.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/StdioPrinter.h>

int main(int argc, char **argv)
{
	RandInit();

	CxxTest::StdioPrinter tmp;
	XTimeCounter t;
	int status = CxxTest::Main<CxxTest::StdioPrinter>( tmp, argc, argv );
	printf("Test finished in %d ms\n", t.Elapsed());
	return status;
}
