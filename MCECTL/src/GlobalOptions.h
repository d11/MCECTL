/*
 * =====================================================================================
 *
 *       Filename:  GlobalOptions.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/10 21:19:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _GLOBAL_OPTIONS_H_
#define _GLOBAL_OPTIONS_H_

class GlobalOptions {

   bool _quitting;
public:
   GlobalOptions() : _quitting(false) { };
   void SetQuitting() { _quitting = true; };
   bool IsQuitting() { return _quitting; };
};

#endif
