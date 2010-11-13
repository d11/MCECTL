/*
 * =====================================================================================
 *
 *       Filename:  Quit.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/11/10 14:02:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _QUIT_COMMAND_H_
#define _QUIT_COMMAND_H_

#include "Command.h"

namespace Command {

   class QuitCommand : public Command {
   public:
      QuitCommand() { }
      virtual string ToString() const {
         return ":quit";
      }
      virtual void Execute(Environment &environment, GlobalOptions &options) const {
         options.SetQuitting();
      }
   };

}

#endif
