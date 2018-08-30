#include <stdio.h>
int main(int argc, char **argv)
{
	int n, i ;
	unsigned long long factorial ;

	// show error if no argument is passed
	if (argc < 2)
	{
		printf("Error! At least one argument needed.\n");
		return 2;
	}

	else
	{
		factorial = 1;
		n = atoi(argv[1]);

		// show error if the user enters a negative integer
		if (n < 0)
		{
			printf("Error! Factorial of a negative number doesn't exist.\n");
			return 2;
		}

		for(i=1; i <= n; ++i)
		{
			factorial *= i;              // factorial = factorial*i;
		}

		printf("Factorial of %d = %llu \n", n, factorial);
	}

	return 0;
}
