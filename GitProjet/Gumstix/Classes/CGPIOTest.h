
/*
 *  Copyright (C) 2012  plegal  (plegal@appert44.org)
 *  @file         CGPIOTest.h
 *  Classe        CGPIO
 *  @note         Classe en charge des tests unitaires
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CGPIOTEST_H
#define _CGPIOTEST_H

// Includes system C

// Includes system C++
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// Includes qt

// Includes application
#include "CGPIO.h"

class CGPIOTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CGPIOTest);
    //CPPUNIT_TEST(testConstructor);
    CPPUNIT_TEST(testUnitaire1);
    CPPUNIT_TEST_SUITE_END();

public :
    void setUp();
    void tearDown();

   void testConstructor();
    void testUnitaire1();

protected :
    // Attributs proteges

    // Methode protegees

private :
    // Attributs prives

    // Methodes privees
};

#endif  // _CGPIOTEST_H


