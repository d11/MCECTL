/*
 * =====================================================================================
 *
 *       Filename:  Command.h
 *
 *    Description:  
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */
#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>

struct Environment;
struct GlobalOptions;

using namespace std;

namespace Command {

   class Command {
      public:
         virtual string ToString() const = 0;
         virtual void Execute(Environment &environment, GlobalOptions &options) const = 0;
         virtual ~Command() { };
   };

   typedef Command *CommandRef;
 
}
#endif
