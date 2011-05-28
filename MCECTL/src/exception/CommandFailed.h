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
      string _message;
      static inline string FormMessage(const string &message) {
         return string("Command failed: ") + message;
      }
   public:
      CommandFailed(const char *message) : _message(FormMessage(message)) {}
      CommandFailed(const string &message) : _message(FormMessage(message)) {}
      virtual const char* what() const throw () {
         return _message.c_str();
      }
      virtual ~CommandFailed() throw () { }
};

#endif
