/*
 * =====================================================================================
 *
 *       Filename:  RunCheck.h
 *    Description:  Do some model-checking
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */

#ifndef  _RUNCHECK_H_
#define  _RUNCHECK_H_

#include "Command.h"
#include "Environment.h"
#include "ModelChecker.h"

using namespace std;

namespace Command {

    class RunCheck : public Command {
      private:
         string system_name;
         string formula_name;
      public:
         RunCheck(const string &system_name, const string &formula_name )
            : _system_name(system_name), _formula_name(formula_name)
         { }

         virtual string ToString() const {
            stringstream s;
            s << "CHECK " << _system_name << " |= " << _formula_name;
            return s.str();
         }
         virtual void Execute(Environment &environment, GlobalOptions &options) {
            TransitionSystem transition_system = environment.GetSystem(_system_name);

            ModelChecker model_checker(environment, transition_system);

            Formula::Formula::const_reference formula = environment.GetFormula(_formula_name);
            CheckResults results = model_checker.Check(formula);
            cout << CheckResults.ToString() << endl;
         }
   };

}


#endif
