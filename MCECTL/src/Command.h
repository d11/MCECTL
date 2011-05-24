/*
 * =====================================================================================
 *
 *       Filename:  Command.h
 *    Description:  Base class for command objects
 *
 * =====================================================================================
 */
#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>

#include "Showable.h"

struct Environment;
struct GlobalOptions;

using namespace std;

namespace Command {

   class Command : public Showable {
      public:
         virtual void Execute(Environment &environment, GlobalOptions &options) = 0;
         virtual ~Command() { };
   };

   typedef Command *CommandRef;
 
}
#endif
