/*
 * =====================================================================================
 *
 *       Filename:  LoadFile.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef _LOAD_FILE_COMMAND_H_
#define _LOAD_FILE_COMMAND_H_

#include "Command.h"

#include <cstdio>
#include <vector>
#include "CTL.h"
#include "exception/CommandFailed.h"
#include "exception/ParseException.h"
#include "command/CommandProcessor.h"

using namespace std;

namespace Command {

    class LoadFile : public Command {
      private:
         string _filename;
      public:
         LoadFile(const string &filename) : _filename(filename) { };
         virtual string ToString() const { string s = ":load(\""; return s + _filename + "\")"; };
         virtual void Execute(Environment &environment, GlobalOptions &options) {
            FILE *input = fopen(_filename.c_str(), "r");
            if (NULL == input) {
               perror("Couldn't open file");
               throw CommandFailed("Couldn't open file");
            }

            CommandParser parser;
            vector<CommandRef> commands;
            try {
               commands = parser.ParseFile(input);
            } catch (ParseException e) {
               throw CommandFailed(e.what());
            }
            fclose(input);
            //vector<CommandRef>::const_iterator iter;
            //for (iter = commands.begin(); iter != commands.end(); ++iter) {
            //   cout << (*iter)->ToString() << endl;
            //}
            CommandProcessor command_processor(environment, options);
            command_processor.ExecuteCommandList(commands);

         }
   };

}

#endif
