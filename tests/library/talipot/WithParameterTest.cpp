/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include "WithParameterTest.h"

#include <talipot/Coord.h>
#include <talipot/Size.h>
#include <talipot/Color.h>
#include <talipot/DataSet.h>

using namespace tlp;
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(WithParameterTest);

const std::string intParameter = "integerParameter";
const std::string stringParameter = "stringParameter";
const std::string longParameter = "longParameter";
const std::string uintParameter = "uintegerParameter";
const std::string doubleParameter = "doubleParameter";
const std::string floatParameter = "floatParameter";
const std::string boolParameter = "booleanParameter";
const std::string CoordParameter = "coordParameter";
const std::string SizeParameter = "sizeParameter";
const std::string ColorParameter = "colorParameter";

typedef uint uint;

#define EXPECTED_VALUE(TYPE, VALUE)              \
    TYPE TYPE##Value = TYPE();                   \
    set.get<TYPE>(TYPE##Parameter, TYPE##Value); \
    CPPUNIT_ASSERT_EQUAL(VALUE, TYPE##Value)

void WithParameterTest::testBuildDefaultDataSet() {
    WithParameter *parametrizedStuff = new ParameteredClass();

    DataSet set;
    parametrizedStuff->getParameters().buildDefaultDataSet(set);

    CPPUNIT_ASSERT(set.exists(intParameter));
    CPPUNIT_ASSERT(set.exists(longParameter));
    CPPUNIT_ASSERT(set.exists(stringParameter));
    CPPUNIT_ASSERT(set.exists(uintParameter));
    CPPUNIT_ASSERT(set.exists(doubleParameter));
    CPPUNIT_ASSERT(set.exists(floatParameter));
    CPPUNIT_ASSERT(set.exists(boolParameter));
    CPPUNIT_ASSERT(set.exists(SizeParameter));
    CPPUNIT_ASSERT(set.exists(ColorParameter));

    EXPECTED_VALUE(int, 42);
    EXPECTED_VALUE(long, 420000000l);
    EXPECTED_VALUE(string, string("forty two"));
    EXPECTED_VALUE(uint, 24u);
    EXPECTED_VALUE(double, 4.2);
    EXPECTED_VALUE(float, 2.4f);
    EXPECTED_VALUE(bool, true);
    EXPECTED_VALUE(Coord, Coord(4.4f, 2.0f, 1.1f));
    EXPECTED_VALUE(Size, Size(2.2f, 1.1f, 4.0f));
    EXPECTED_VALUE(Color, Color(1, 4, 255, 9));

    delete parametrizedStuff;
}

ParameteredClass::ParameteredClass() {
    addInParameter<int>(intParameter, "this is a test integer parameter", "42");
    addInParameter<long>(longParameter, "this is a test long parameter", "420000000");
    addInParameter<string>(stringParameter, "this is a test string parameter", "forty two");
    addInParameter<uint>(uintParameter, "this is a test unsigned integer parameter", "24");
    addInParameter<double>(doubleParameter, "this is a test double parameter", "4.2");
    addInParameter<float>(floatParameter, "this is a test float parameter", "2.4");
    addInParameter<bool>(boolParameter, "this is a test boolean parameter", "true");
    addInParameter<Coord>(CoordParameter, "this is a test Coord parameter", "(4.4, 2, 1.1)");
    addInParameter<Size>(SizeParameter, "this is a test Size parameter", "(2.2, 1.1, 4)");
    addInParameter<Color>(ColorParameter, "this is a test Color parameter", "(1, 4, 255, 9)");
}
