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

      set<string> v1;
      v1.insert("p");
      v1.insert("q");
      set<string> v2;
      v2.insert("p");

      vector<KripkeState*> states;
      KripkeState *s1 = new KripkeState("foo", v1);
      KripkeState *s2 = new KripkeState("bar", v2);
      states.push_back(s1);
      states.push_back(s2);

      KripkeStructure ts(states, s1);

      //StackSymbol sym("ó");
      //PushAction *action = new PushAction("a", sym, sym);
      RegularAction *action = new RegularAction("a");

      ts.AddRule(s1, action, s2);
      ModelChecker mc(env, ts);

      cout << ts.ToString() << endl;
   );
}

BOOST_AUTO_TEST_SUITE_END()

