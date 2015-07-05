#include <stdio.h>

int main(int argc, char** argv) {
	if (argc == 1)
	{
		char c = argv[0][0];
		printf("%c\n", c);
		printf("Hello %s!\n", argv[0]);
	}
	else
	{
		printf("Hello World!\n");
	}
	return 0;
}
