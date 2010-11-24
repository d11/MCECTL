/*
 * =====================================================================================
 *
 *       Filename:  Quit.h
 *    Description:  Command which signals program exit
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
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
      virtual void Execute(Environment &environment, GlobalOptions &options) {
         options.SetQuitting();
      }
   };

}

#endif
