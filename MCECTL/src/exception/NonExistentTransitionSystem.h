/*
 * =====================================================================================
 *
 *       Filename:  NonExistentTransitionSystem.h
 *    Description:  Tried to look up a transition_system that doesn't exist
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */


#ifndef  _NONEXISTENTTRANSITIONSYSTEM_H_
#define  _NONEXISTENTTRANSITIONSYSTEM_H_
#include <exception>
#include <string>

class NonExistentTransitionSystemException : public exception
{
   private:
      string _transition_system_name;
   public:
      NonExistentTransitionSystemException(const string &transition_system_name) : _transition_system_name(transition_system_name) { }
      virtual const char* what() const throw () {
         string s = "Couldn't find transition_system '";
         s += _transition_system_name + "'";
         return s.c_str();
      }
      virtual ~NonExistentTransitionSystemException() throw () { }
};

#endif

