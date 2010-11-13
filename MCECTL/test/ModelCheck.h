/*
 * =====================================================================================
 *
 *       Filename:  ModelCheck.h
 *    Description:  Tests for the model checker
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include "ModelChecker.h"


BOOST_AUTO_TEST_SUITE( test_suite_model_checker )

BOOST_AUTO_TEST_CASE( test_basic )
{
   BOOST_CHECK_NO_THROW(
      Environment env;
      TransitionSystem ts;
      ModelChecker mc(env, ts);
   );
}

BOOST_AUTO_TEST_SUITE_END()

