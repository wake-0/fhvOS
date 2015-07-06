#include <stdio.h>

int main(int argc, char** argv) {
	printf("Hi!\n");
	printf("I'm a pretty useless program, actually I'm just trying to access a null pointer...\n\n");
	printf("Sincerely,\n");

	printf("A null pointer exception\n");
	volatile char* xx = 0;
	xx[0] = '\0';

	printf("PS: I's NOT working! q.e.d.\n");
	return 0;
}
