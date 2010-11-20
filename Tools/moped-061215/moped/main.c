/*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <wpds.h>

#include "bdd.h"
#include "ir.h"
#include "bp.h"
#include "dpn.h"
#include "chkLang.h"

#include <time.h>

#define isdigit(c) ((c >= '0') && (c <= '9'))

int ABSTRACTION = 0;
int DPN = 0;
int BOUND = 0;
int NEWTRACES = 0;
int absVerbose = 0; /* Remy: I add this for controlling Kiefer verbosity option (-kv).
			Change it as you wish. */
extern int twoVerbose;

static void usage (char *progname)
{
	printf("usage: %s [options] <file> <label>\n\n"
	       "Checks if <label> is reachable.\n"
	       "<file> must be a boolean program or a Remopla file "
	       "(depending on whether option -b is given or not).\n"
	       "    Options:\n"
	       "    -b\t   check a boolean program "
	       "(<label> may be omitted if this option is given)\n"
	       "    -A\t   enable CEGAR\n"
	       "    -B\t   use pre* method (backwards)\n"
	       "    -T\t   use new method for trace generation\n",
	       progname);
	exit(1);
}

void parseCommandLine(int argc, char **argv, int *method, int *inputType,
		      char **filename, char **stackSymbol)
{
	int i;
	for (i = 1; i < argc; i++)
        {
		if (*argv[i] == '-')
		{
			while (*++argv[i])
				switch (*argv[i])
				{
				case 'A': /* abstraction */
					ABSTRACTION = 1;
					break;
				case 'B': { /* pre* method */
					*method = 1;
					if (isdigit(argv[i][1])) {
						*method = argv[i][1] - '0';
						argv[i]++;
					}
					break;
				}
				case 'b': /* boolean program */
					*inputType = 1;
					break;

				case 'c': /*check modus*/
					*inputType = 2;
                   			break;
				
				case 'D': { /* DPN */
					DPN = 1;
					int j = 1;
					while (isdigit(argv[i][j])) {
						BOUND = (BOUND * 10) + (argv[i][j] - '0');
						j++;
					}
					argv[i] += (j - 1);
					break;
				}
				case 'F': { /* post* method */
					*method = 0;
					if (argv[i][1] == '2') {
						*method = 3;
						argv[i]++;
					}
					break;
				}
				case 'T': /* traces */
					NEWTRACES = 1;
					break;
				case 'v': /* verbose */
					switch (argv[i][1]) 
					{
					case 'r': /* Remy */
						if (isdigit(argv[i][2]))
							irVerbose = argv[i][2] - '0';
						else usage(argv[0]);
						argv[i] += 2;
						break;
					case 'k': /* Kiefer */
						/* Remy: Change as you wish */
						absVerbose = 1;
						argv[i] += 1;
						break;
					case 's': /* Schwoon */
						/* Remy: Change as you wish */
						break;
					case 't': /* for procedures in two* */
						if (isdigit(argv[i][2]))
							twoVerbose = argv[i][2] - '0';
						else usage(argv[0]);
						argv[i] += 2;
						break;
					default: usage(argv[0]);
					}
					break;

				default:  usage(argv[0]);
				}
		}
		else
		{
			if (*filename == 0) 
				*filename = argv[i];
			else
				*stackSymbol = argv[i];
		}
	}

	/*
	printf("filename:%s, stackSymbol:%s, inputType:%d\n",
	       *filename, *stackSymbol, *inputType);
	*/       

	if (!(*filename) || ((*inputType == 0) && !(*stackSymbol))) usage(argv[0]);
	if (ABSTRACTION && (*method > 0))
	{
		printf("Currently, you cannot use options -A and -B together.\n");
		exit(1);
	}
}

int main (int argc, char **argv)
{
	intPds *ipds;
	intResult *iresult;
	intProblem iproblem;
	
	bddPds *bpds;
	bddResult *bresult;
	bddProblem bproblem;

	int exitStatus;
	
	char *filename = 0;
	char *stackSymbol = 0;
	
	clock_t startTime, endTime;
	time_t startTime2, endTime2;
	double elapsed;
	startTime = clock();
	startTime2 = time(NULL);

	int method    = 0; /* post* if not changed by option -B */
	int inputType = 0; /* Remopla if not changed by option -b */
	
	parseCommandLine(argc,argv,&method,&inputType,&filename,&stackSymbol);
	
	wInit();
	bddInit();

	if (inputType == 0) /* Remopla */
	{
		iproblem.state = wIdentCreate("q");
		iproblem.stack = wIdentCreate(stackSymbol);

		ipds = irReadPds(filename, iproblem.stack);

		if (absVerbose) {
			printf("num_globals: %i\n", ipds->pds->num_globals);
			printf("num_locals:  %i\n", ipds->pds->num_locals);
		}

		if (DPN) {
			iproblem.type = (BOUND > 0) 
				? PROBLEM_REACHABILITY_ADPN
				: PROBLEM_REACHABILITY_DPN;
		} else {
			switch (method) {
			case 0: iproblem.type = PROBLEM_REACHABILITY;
				break;
			case 1: iproblem.type = PROBLEM_REACHABILITY_PRE;
				break;
			case 2: iproblem.type = PROBLEM_REACHABILITY_PRE_2;
				break;
			case 3: iproblem.type = PROBLEM_REACHABILITY_2;
				break;
			default: usage(argv[0]);
			}
		}
		
		iresult = intAnalyse(ipds,&iproblem);
		
		if (!ABSTRACTION)
			intPrintResult(ipds,iresult);
		
		exitStatus = iresult->yesno == RESULT_YES ? 111 : 112;
		intDeleteResult(ipds,iresult);
		intPdsDelete(ipds);
	} else if (inputType == 1) /* boolean program */
	{
		bpds = bpReadBP(argv[argc-1]);
		printf("num_globals: %i\n", bpds->num_globals);
		printf("num_locals:  %i\n", bpds->num_locals);
		
		bproblem.type = method
			? PROBLEM_REACHABILITY_PRE 
			: PROBLEM_REACHABILITY;
		bproblem.state = wIdentCreate("q");
		
		bp_fun_t fun;
		char tmps[256]; // fixme
		for (fun = bp_functions; fun; fun = fun->next)
		{
			sprintf(tmps,"%s:%s",wIdentString(fun->funname),"SLIC_ERROR");
			mc_reach_target_label = bproblem.stack = wIdentCreate(tmps);
			if (bp_lookup(bp_labeltree,bproblem.stack,-2)) break;
		}
		
		if (!fun)
		{
			fprintf(stderr,"label SLIC_ERROR not found\n");
			exit(1);
		}

		bresult = bddAnalyse(bpds,&bproblem);
		
		if (!ABSTRACTION)
			bddPrintResult(bpds,bresult);

		exitStatus = bresult->yesno == RESULT_YES ? 111 : 112;
		bddDeleteResult(bpds,bresult);
		bddPdsDelete(bpds);
	}
	else if (inputType == 2) //check modus
	{
		printf("input file = %s", filename);
		chkParse(filename);
	}
	
		
	int eins,zwei,drei;
	if (absVerbose) {
		printf("-----------------------------------------------------\n");
		printf("Debugging Info:\n");
		bddDebug(&eins, &zwei, &drei);
		printf("eins: %d\n", eins);
		printf("zwei: %d\n", zwei);
		printf("drei: %d\n", drei);
	}

	bddFinish();
	wFinish();
	endTime = clock();
	endTime2 = time(NULL);
	elapsed = ((double) (endTime - startTime)) / CLOCKS_PER_SEC;
	fprintf(stderr, "%1.2f(%ld) sec   ", elapsed, (long) (endTime2 - startTime2));
	printf("\n");

	if (absVerbose && (eins | zwei | drei)) return 1;
	else return exitStatus;
}
