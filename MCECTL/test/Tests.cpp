/*
 * =====================================================================================
 *
 *       Filename:  Tests.cpp
 *    Description:  Main test harness
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TestMCECTL
#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;

#include "test/Parse.h"
#include "test/ModelCheck.h"

BOOST_AUTO_TEST_SUITE( test_suite_general )
BOOST_AUTO_TEST_CASE( test_general )
{
    BOOST_CHECK( 1 );
}

BOOST_AUTO_TEST_SUITE_END()

