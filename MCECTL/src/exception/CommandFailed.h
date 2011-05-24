/*
 * =====================================================================================
 *
 *       Filename:  CommandFailed.h
 *    Description:  Generic exception for when a command has not completed correctly
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
      CommandFailed(const string &message) : _parse_error(message) { }
      virtual const char* what() const throw () {
         string s = "Command failed: ";
         s += _parse_error;
         return s.c_str();
      }
      virtual ~CommandFailed() throw () { }
};

#endif
