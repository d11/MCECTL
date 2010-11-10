/*
 * =====================================================================================
 *
 *       Filename:  CommandFailed.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/11/10 01:40:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _COMMAND_FAILED_H_
#define _COMMAND_FAILED_H_

#include <exception>
#include <string>

class CommandFailed : public exception
{
   private:
      string _parse_error;
   public:
      CommandFailed(const char *message) : _parse_error(message) { }
      virtual const char* what() const throw ()
      {
         string s = "Syntax error: ";
         s += _parse_error;
         return s.c_str();
      }
      virtual ~CommandFailed() throw () { }
};

#endif
