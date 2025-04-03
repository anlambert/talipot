#include <talipot/Graph.h>
#include <talipot/TlpTools.h>

#include "CppUnitIncludes.h"

using namespace std;

class TlpToolsTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(TlpToolsTest);
    CPPUNIT_TEST(testDemangleClassName);
    CPPUNIT_TEST(testDemangleTlpClassName);
    CPPUNIT_TEST_SUITE_END();

  public:
    void testDemangleClassName() {
        CPPUNIT_ASSERT_EQUAL(string("tlp::Graph"),
                             tlp::demangleClassName(typeid(tlp::Graph).name()));
        CPPUNIT_ASSERT_EQUAL(string("tlp::Graph"), tlp::demangleClassName<tlp::Graph>());
    }

    void testDemangleTlpClassName() {
        CPPUNIT_ASSERT_EQUAL(string("Graph"), tlp::demangleTlpClassName(typeid(tlp::Graph).name()));
        CPPUNIT_ASSERT_EQUAL(string("Graph"), tlp::demangleTlpClassName<tlp::Graph>());
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION(TlpToolsTest);