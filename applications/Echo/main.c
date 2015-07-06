#include <stdio.h>

int main(int argc, char** argv) {
	if (argc == 0)
	{
		printf("Usage:\n\t[some string]\t\tWill print out the given string.\n");
		return 0;
	}

	int i = 0;
	for (i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
	return 0;
}
