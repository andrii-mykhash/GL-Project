#include "inc/dot_test.h"
#include "inc/field_test.h"
#include "inc/json_wrapper_test.h"

#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TextTestRunner.h>

CPPUNIT_TEST_SUITE_REGISTRATION( DotTest );
CPPUNIT_TEST_SUITE_REGISTRATION( FieldTest );
CPPUNIT_TEST_SUITE_REGISTRATION( JsonWrapperTest );

int main()
{
    CPPUNIT_NS::TestResult testresult;
    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);
    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener (&progress);
    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
    testrunner.run(testresult);
    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write ();
 
    return collectedresults.wasSuccessful() ? 0 : 1;
}