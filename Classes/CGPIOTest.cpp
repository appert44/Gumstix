/*
 *  Copyright (C) 2012  plegal  (plegal@appert44.org)
 *  @file         CGPIOTest.cpp
 *  Classe        CGPIO
 *  @note         Implementation de la classe en charge des tests unitaires
 */

// Includes system C
#include <stdlib.h>
// Includes system C++
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>

#include <iostream>
using namespace std;

// Includes qt

// Includes application
#include "CGPIOTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(CGPIOTest);

// Exemple d'assertions possibles
// # CPPUNIT_ASSERT(condition)
// Assertions that a condition is true.
// # CPPUNIT_ASSERT_MESSAGE(message, condition)
// Assertion with a user specified message.
// # CPPUNIT_FAIL(message)
// Fails with the specified message
// # CPPUNIT_ASSERT_EQUAL(expected, actual)
// Asserts that two values are equals.
// # CPPUNIT_ASSERT_EQUAL_MESSAGE(message, expected, actual)
// Asserts that two values are equals, provides additional message on failure
// # CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, delta)

// setUp() to initialize the variables you will use for test
void CGPIOTest::setUp() {
}

// tearDown() to release any permanent resources you allocated in setUp()
void CGPIOTest::tearDown() {
}

// Suite des tests unitaires

void CGPIOTest::testConstructor() {
	// Construction de l'instance de classe a tester
	CGPIO *cgpio = new CGPIO((char *) "/dev/gpio064", OUTPUT, 0);
	CPPUNIT_ASSERT(cgpio != NULL);
	delete cgpio;
}

void CGPIOTest::testUnitaire1() {
	CGPIO *cgpio64 = new CGPIO((char *) "/dev/gpio064", OUTPUT, 0);

		CGPIO *cgpio86 = new CGPIO((char *) "/dev/gpio086", INPUT, 0);

		CPPUNIT_ASSERT(cgpio64 != NULL);
		CPPUNIT_ASSERT(cgpio86 != NULL);
		// Test unitaire d'une methode publique de la classe
		// Utilisation des macros CPPUNIT_ASSERT, CPPUNIT_ASSERT_EQUAL, etc.
		int ret = cgpio64->Open();
		cout << "open : " << ret << endl;
		CPPUNIT_ASSERT_MESSAGE("GPIO 64 Init", ret == 0);

		ret = cgpio86->Open();
		cout << "open : " << ret << endl;
		CPPUNIT_ASSERT_MESSAGE("GPIO 86 Init", ret == 0);

	/*	for (int i = 0; i < 4; i++) {

			ret = cgpio64->Write(1);
			cout << "write : " << ret << endl;
			CPPUNIT_ASSERT_MESSAGE("GPIO Write", ret > 0);

			sleep(1);

			ret = cgpio64->Write(0);
			cout << "write : " << ret << endl;

			CPPUNIT_ASSERT_MESSAGE("GPIO Write", ret > 0);

			sleep(1);
		}*/

		while (1)
		{
			usleep(100000);
			ret = cgpio86->Read();
			cgpio64->Write(ret);
		}

		delete cgpio64;
		delete cgpio86;

	}


// the main method
int main(int argc, char* argv[]) {
	// informs test-listener about testresults
	CPPUNIT_NS::TestResult testresult;

	// register listener for collecting the test-results
	CPPUNIT_NS::TestResultCollector collectedresults;
	testresult.addListener(&collectedresults);

	// register listener for per-test progress output
	CPPUNIT_NS::BriefTestProgressListener progress;
	testresult.addListener(&progress);

	// insert test-suite at test-runner by registry
	CPPUNIT_NS::TestRunner testrunner;
	testrunner.addTest(
			CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
	testrunner.run(testresult);

	// output results in compiler-format
	CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults,
			std::cerr);
	compileroutputter.write();

	// for hudson
	std::ofstream file("CGPIO-cppunit-report.xml");
	CPPUNIT_NS::XmlOutputter xmloutputter(&collectedresults, file);
	xmloutputter.write();
	file.close();

	// return 0 if tests were successful
	return collectedresults.wasSuccessful() ? 0 : 1;
}

