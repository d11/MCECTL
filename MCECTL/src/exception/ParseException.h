/*
 * =====================================================================================
 *
 *       Filename:  ParseException.h
 *    Description:  
 *
 * =====================================================================================
 */
#ifndef _PARSE_EXCEPTION_H_
#define _PARSE_EXCEPTION_H_

#include <exception>
#include <string>

class ParseException : public exception
{
   private:
      string _parse_error;
   public:
      ParseException(const char *message) : _parse_error(message) { }
      virtual const char* what() const throw ()
      {
         string s = "Syntax error: ";
         s += _parse_error;
         return s.c_str();
      }
      virtual ~ParseException() throw () { }
};

#endif
