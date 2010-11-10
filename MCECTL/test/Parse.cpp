/*
 * =====================================================================================
 *
 *       Filename:  Parse.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/11/10 12:13:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "CTL.h"
#include "exception/ParseException.h"
#include "Command.h"
using namespace Command;

#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
   CommandParser parser;

   const char *test_cases[] = {
      // Regex
      "REGULAR foo { toast }",
      "REGULAR foo { (toast) }",
      "REGULAR foo { ( toast ) }",
      "REGULAR foo { toast* }",
      "REGULAR foo { . }",
      "REGULAR foo { .*  }",
      "REGULAR foo { toast|coffee }",
      "REGULAR foo { (toast|coffee) }",
      "REGULAR foo { (toast|coffee)* }",
      "REGULAR foo { toast   eat }",
      "REGULAR foo { toast*  eat }",
      "REGULAR foo { toast*  (eat|dance) }",
      "REGULAR foo { toast*  (eat|dance)* }",
      "REGULAR foo { toast*  ((party|sing*)eat|dance)* }",

      // Formula
      "FORMULA foo { a }",
      "FORMULA foo { (a & b) }",
      "FORMULA foo { (toast & jam) }",
      "FORMULA foo { (toast |jam) }",
      "FORMULA foo { (jam -> jam) }",
      "FORMULA foo { E(hungry U eat) }",
   };

   int count = sizeof(test_cases)/sizeof(char*);
   int i;
   int fail_count = 0;
   for (i = 0; i < count; ++i) {
      cout << i << ". " << test_cases[i] << " ........................... ";
      try {
         string input = test_cases[i];
         vector<CommandRef> commands(parser.ParseString(input + "\n"));

         CommandRef command(*commands.begin());

         string cmd_string = command->ToString();
         if (cmd_string.compare(test_cases[i]) == 0) {
            cout << "OK" << endl;
         }
         else {
            cout << "FAILED" << endl;
            cout << "Got: [ " << endl;
            cout << cmd_string << endl;
            cout << "]" << endl;
         }
      } catch (ParseException e) {
         ++fail_count;
         cout << "FAILED" << endl;
         cout << e.what() << endl;
      }
      cout << endl;
   }

}
