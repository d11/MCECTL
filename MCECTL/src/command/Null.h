/*
 * =====================================================================================
 *
 *       Filename:  Null.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/11/10 14:04:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
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
      virtual void Execute(Environment &environment, GlobalOptions &options) const {
      }
   };
}

#endif
