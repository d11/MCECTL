/*
 * =====================================================================================
 *
 *       Filename:  DFABuilder.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/11/10 10:27:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _DFABUILDER_H_
#define _DFABUILDER_H_

#include <vector>
#include "Automata.h"

using namespace std;

template <class T>
class DFABuilder {
private:
public:
   typedef boost::shared_ptr< DFA<T> > dfa_ref;

   DFABuilder() {} 

   dfa_ref Build() {

      vector< DFA<T>::node_ref > nodes;
      dfa_ref dfa(new DFA<T>(nodes), initial_node);
      return dfa;
   }
};

#endif
