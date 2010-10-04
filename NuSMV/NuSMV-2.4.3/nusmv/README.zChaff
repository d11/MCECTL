zChaff is a powerful SAT Solver [1] that can be optionally used with
NuSMV to perform BMC. 

For information about how to compile and link zChaff within NuSMV, and
for other important information about zChaff, please refer to the file
<NuSMV-(version)/zchaff/README> distributed along with the NuSMV
source code.


******************************************************************************
*                           (0) zChaff License                               *
******************************************************************************
Before using zChaff with NuSMV, you have to know that: 

    *****************************************************************
    *** zChaff is for non-commercial purposes only.               ***
    *** NO COMMERCIAL USE OF ZCHAFF IS ALLOWED WITHOUT WRITTEN    ***
    *** PERMISSION FROM PRINCETON UNIVERSITY.                     ***
    *** Please contact Sharad Malik (malik@ee.princeton.edu)      ***
    *** for details.                                              ***
    *****************************************************************


******************************************************************************
*                        (1) Instruct NuSMV to use zChaff                    *
******************************************************************************
To exploit the zChaff solver, you have to enter an interactive 
NuSMV session:

    NuSMV -int yourModel.smv

issue the command:

    set sat_solver "zchaff"
    
and then proceed with your BMC session. zChaff will be used to solve
the generated instances.

Since version 2.3.1, command line option "-sat_solver" is provided to 
set the SAT solver in batch mode as well. 



[1] See <http://www.princeton.edu/~chaff/zchaff.html>
