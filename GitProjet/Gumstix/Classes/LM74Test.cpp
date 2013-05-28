
/*
 *  Copyright (C) 2013  plegal  (plegal@appert44.org)
 *  @file         LM74Test.cpp
 *  Classe        LM74
 *  @note         Implementation de la classe en charge des tests unitaires
 */

// Includes system C

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
#include "LM74Test.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(LM74Test);

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
void LM74Test::setUp()
{
}

// tearDown() to release any permanent resources you allocated in setUp()
void LM74Test::tearDown()
{
}

// Suite des tests unitaires

void LM74Test::testConstructor()
{
//    // Construction de l'instance de classe a tester
//    LM74 *lm74 = new LM74("/dev/spike");
//    CPPUNIT_ASSERT(lm74 != NULL);
//    delete lm74;
}

void LM74Test::testUnitaire1()
{
    // Construction de l'instance de classe a tester
    LM74 *lm74 = new LM74("/dev/spike");
    CPPUNIT_ASSERT(lm74 != NULL);
    // Test unitaire d'une methode publique de la classe
    // Utilisation des macros CPPUNIT_ASSERT, CPPUNIT_ASSERT_EQUAL, etc.
    cout << "open spike" << endl;
    lm74->Open();

    //lm74->SetShutdownMode();
    //sleep(1);
    //cout << "set continous conversion" << endl;
  // lm74->SetContConvMode();



    	cout << "Temperature : " << lm74->Read() << endl;
    	sleep(1);

    lm74->Close();
    delete lm74;
}

// the main method
int main(int argc, char* argv[])
{
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
    testrunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    testrunner.run(testresult);

    // output results in compiler-format
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write();

    // for hudson
    std::ofstream file( "LM74-cppunit-report.xml" );
    CPPUNIT_NS::XmlOutputter xmloutputter(&collectedresults, file);
    xmloutputter.write();
    file.close();

    // return 0 if tests were successful
    return collectedresults.wasSuccessful() ? 0 : 1;
}

