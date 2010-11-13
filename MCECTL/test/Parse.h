/*
 * =====================================================================================
 *
 *       Filename:  Parse.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#include "CTL.h"
#include "exception/ParseException.h"
#include "Command.h"
using namespace Command;

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include <boost/test/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
using namespace boost::unit_test;

CommandParser parser;

typedef pair<string,string> test_case_t;

test_case_t const test_cases[] = {

   // Regex
   test_case_t("REGULAR foo { toast }",
               "REGULAR foo { [ACTION toast] }"),

   test_case_t("REGULAR foo { (toast) }",
               "REGULAR foo { [ACTION toast] }"),

   test_case_t("REGULAR foo { ( toast ) }",
               "REGULAR foo { [ACTION toast] }"),

   test_case_t("REGULAR foo { toast* }",
               "REGULAR foo { [KLEENE [ACTION toast]] }"),

   test_case_t("REGULAR foo { . }",
               "REGULAR foo { [ANY] }"),

   test_case_t("REGULAR foo { .*  }",
               "REGULAR foo { [KLEENE [ANY]] }"),

   test_case_t("REGULAR foo { toast|coffee }",
               "REGULAR foo { [[ACTION toast] UNION [ACTION coffee]] }"),

   test_case_t("REGULAR foo { (toast|coffee) }",
               "REGULAR foo { [[ACTION toast] UNION [ACTION coffee]] }"),

   test_case_t("REGULAR foo { (toast|coffee)* }",
               "REGULAR foo { [KLEENE [[ACTION toast] UNION [ACTION coffee]]] }"),

   test_case_t("REGULAR foo { toast   eat }",
               "REGULAR foo { [[ACTION toast] CONCAT [ACTION eat]] }"),

   test_case_t("REGULAR foo { toast*  eat }",
               "REGULAR foo { [[KLEENE [ACTION toast]] CONCAT [ACTION eat]] }"),

   test_case_t("REGULAR foo { toast*  (eat|dance) }",
               "REGULAR foo { [[KLEENE [ACTION toast]] CONCAT [[ACTION eat] UNION [ACTION dance]]] }"),

   test_case_t("REGULAR foo { toast*  (eat|dance)* }",
               "REGULAR foo { [[KLEENE [ACTION toast]] CONCAT [KLEENE [[ACTION eat] UNION [ACTION dance]]]] }"),

   test_case_t("REGULAR foo { toast*  ((party|sing*)eat|dance)* }",
               "REGULAR foo { [[KLEENE [ACTION toast]] CONCAT [KLEENE [[[[ACTION party] UNION [KLEENE [ACTION sing]]] CONCAT [ACTION eat]] UNION [ACTION dance]]]] }"),

   // Formula
   test_case_t("FORMULA foo { a }",
               "FORMULA foo { [PVAR a] }"),

   test_case_t("FORMULA foo { (a & b) }",
               "FORMULA foo { [[PVAR a] AND [PVAR b]] }"),

   test_case_t("FORMULA foo { (toast & jam) }",
               "FORMULA foo { [[PVAR toast] AND [PVAR jam]] }"),

   test_case_t("FORMULA foo { (toast |jam) }",
               "FORMULA foo { [[PVAR toast] OR [PVAR jam]] }"),

   test_case_t("FORMULA foo { ( jam-> jam) }",
               "FORMULA foo { [[PVAR jam] IMPLIES [PVAR jam]] }"),

   test_case_t("FORMULA foo { E(hungry U[dfa] eat) }",
               "FORMULA foo { [[PVAR hungry] UNTIL {AUTOMATA dfa} [PVAR eat]] }"),

};

void test_parse_one( const test_case_t &input ) {
   BOOST_CHECK_NO_THROW(
      vector<CommandRef> commands(parser.ParseString(input.first + "\n"));
      CommandRef command(*commands.begin());
      string cmd_string = command->ToString();
      BOOST_CHECK_EQUAL( cmd_string, input.second );
   );
}

BOOST_AUTO_TEST_SUITE( test_suite_parsing )

BOOST_AUTO_TEST_CASE( test_parsing )
{
   size_t test_count = sizeof(test_cases)/sizeof(test_case_t);
   for_each(test_cases, test_cases+test_count, test_parse_one);
}

BOOST_AUTO_TEST_SUITE_END()
