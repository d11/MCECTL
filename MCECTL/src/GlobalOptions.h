/*
 * =====================================================================================
 *
 *       Filename:  GlobalOptions.h
 *    Description:  Class for storing options needed throughout the program
 *
 * =====================================================================================
 */

#ifndef _GLOBAL_OPTIONS_H_
#define _GLOBAL_OPTIONS_H_

class GlobalOptions {

   bool _quitting;
   bool _verbose;
public:
   GlobalOptions() : _quitting(false), _verbose(false) { }
   void SetVerbose() { _verbose = true; }
   bool IsVerbose() { return _verbose; }
   void SetQuitting() { _quitting = true; }
   bool IsQuitting() { return _quitting; }
};

#endif
