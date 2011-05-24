/*
 * =====================================================================================
 *
 *       Filename:  Showable.h
 *    Description:  Abstract class for objects which can be shown as a string
 *
 * =====================================================================================
 */

#ifndef  _SHOWABLE_H_
#define  _SHOWABLE_H_

#include <string>

using namespace std;

class Showable {
   public:
      virtual string ToString() const = 0;
      virtual ~Showable() {};
};

#endif
