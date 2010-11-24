#include "include.h"
#include "lexical.h"

main(int argc, char** argv)
{
  re<char> r;
  fstream f_arg;
  f_arg.open(argv[1], ios::in);
  if(!f_arg)
    {
      cerr<<"can not open "<<argv[1]<<endl;
      exit(1);
    }
  f_arg>>r;
  
  fm<char> a;
  r.retofm(a);      //transfer r to fm a

  a.subset();       //make a deterministic
  cout<<a;
  return 0;

}
