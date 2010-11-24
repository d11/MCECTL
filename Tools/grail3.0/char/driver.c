#include <stdio.h>
#include <string.h>
#define NUMBER 2

main()
{
	char buf[200];
	char inname[50], outname1[50],outname2[50];
	printf("Enter the dfa file name: ");
	scanf("%s",inname);
	printf("Enter the afa file name for store: ");
	scanf("%s", outname1);
	printf("Enter the afa file name for store the bit representation: ");
	scanf("%s",outname2);

	sprintf(buf, "cat %s|fmtoafa %s %s", inname,outname1,outname2);
	system(buf);
	return 0;
}
