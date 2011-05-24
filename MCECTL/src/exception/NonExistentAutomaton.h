/*
 * =====================================================================================
 *
 *       Filename:  NonExistentAutomaton.h
 *    Description:  Tried to look up an automaton that doesn't exist
 *
 * =====================================================================================
 */


#ifndef  _NONEXISTENTAUTOMATON_H_
#define  _NONEXISTENTAUTOMATON_H_

#include <exception>
#include <string>

class NonExistentAutomatonException : public exception
{
   private:
      string _automaton_name;
   public:
      NonExistentAutomatonException(const string &automaton_name) : _automaton_name(automaton_name) { }
      virtual const char* what() const throw () {
         string s = "Couldn't find automaton '";
         s += _automaton_name + "'";
         return s.c_str();
      }
      virtual ~NonExistentAutomatonException() throw () { }
};

#endif

