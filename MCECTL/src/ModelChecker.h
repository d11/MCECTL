/*
 * =====================================================================================
 *
 *       Filename:  ModelChecker.h
 *
 *    Description:  
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */
#ifndef MODELCHECKER_H
#define MODELCHECKER_H

#include "Environment.h"

class ModelChecker {
private:
   const Environment &_environment;
public:
   ModelChecker(const Environment &environment) 
      : _environment(environment) { }
};

#endif
