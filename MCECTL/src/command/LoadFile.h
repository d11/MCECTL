/*
 * =====================================================================================
 *
 *       Filename:  LoadFile.h
 *    Description:  Load commands from a file
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

				FILE *input;
				// Attempt to open the requested file
				if (_filename == "-") {
					input = stdin;
				} else {
					input = fopen(_filename.c_str(), "r");
				}
				if (NULL == input) {
					string error = "Couldn't open file";
					perror(error.c_str());
					throw CommandFailed((error + " '" + _filename + "'").c_str());
				}

				// Read the list of commands in from the file
            CommandParser parser;
            vector<CommandRef> commands;
            try {
               commands = parser.ParseFile(input);
            } catch (ParseException e) {
               throw CommandFailed(e.what());
            }
            fclose(input);

				// Pass the list to the command processor, for execution
            CommandProcessor command_processor(environment, options);
            command_processor.ExecuteCommandList(commands);

         }
   };

}

#endif
