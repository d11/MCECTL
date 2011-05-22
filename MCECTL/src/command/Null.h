/*
 * =====================================================================================
 *
 *       Filename:  Null.h
 *    Description:  Command which performs no action
 *
 * =====================================================================================
 */

#ifndef _NULL_COMMAND_H_
#define _NULL_COMMAND_H_

#include "Command.h"

namespace Command {
   class NullCommand : public Command {
   public:
      virtual string ToString() const {
         return "";
      }
      virtual void Execute(Environment &environment, GlobalOptions &options)
		{ }
   };
}

#endif
