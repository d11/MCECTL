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
#include "TransitionSystem.h"
#include "Automata.h"

BOOST_AUTO_TEST_SUITE( test_suite_model_checker )

BOOST_AUTO_TEST_CASE( test_basic )
{
   BOOST_CHECK_NO_THROW(
      Environment env;

      vector<string> v1;
      v1.push_back("p");
      v1.push_back("q");
      vector<string> v2;
      v2.push_back("p");

      vector<KripkeState*> states;
      KripkeState *s1 = new KripkeState("foo", "foo", v1);
      KripkeState *s2 = new KripkeState("bar", "bar", v2);
      states.push_back(s1);
      states.push_back(s2);

   //   KripkeStructure ts(states, s1);

      //StackSymbol sym("ó");
      //PushAction *action = new PushAction("a", sym, sym);
      //RegularAction *action = new RegularAction("a");

      //ts.AddRule(s1, action, s2);
     // ts.AddRule(s1, action); //TODO
      //ModelChecker mc(env, ts);

    //  cout << ts.ToString() << endl;
   );
}

BOOST_AUTO_TEST_SUITE_END()

