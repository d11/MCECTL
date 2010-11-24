#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include "include.h"
main()
{

  while(1)
    {
      funct f;
      cout<<"please enter the function: ";
      cin>>f;
      cout<<"the function is: "<<f<<endl;
      f.simplify();
      cout<<"after simplify: "<<f<<endl;
    }
  return 0;
}
