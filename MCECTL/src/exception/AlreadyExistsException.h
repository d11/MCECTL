/*
 * =====================================================================================
 *
 *       Filename:  AlreadyExistsException.h
 *    Description:  Tried to create something with the same name
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */


#ifndef  _ALREADYEXISTSEXCEPTION_H_
#define  _ALREADYEXISTSEXCEPTION_H_

#include <exception>
#include <string>

class AlreadyExistsException : public exception
{
   private:
      string _identifier;
   public:
      AlreadyExistsException(const string &identifier) : _identifier(identifier) { }
      virtual const char* what() const throw () {
         string s = "value for '";
         s += _identifier + "' already exists!";
         return s.c_str();
      }
      virtual ~AlreadyExistsException() throw () { }
};


#endif
