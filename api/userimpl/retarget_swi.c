/*
 * retarget_swi.c
 *
 *  Created on: 03.07.2015
 *      Author: Nicolaj Hoess
 */

#include <stdio.h>


FILE __stdout =     { 0 };
FILE __stdin =      { 1 };

int fputc(int ch, FILE *f)
{
    char tempch = ch;
    switch (tempch) {
    case '\n':
    	//NOP
    	break;
    default:
    	//NOP
    	break;
    }
    return ch;
}

int fputs(const char *s, FILE *f)
{
	int cnt = 0;
	char tmpChar = s[cnt];
	while (tmpChar != '\0') {
		//NOP
		cnt++;
		tmpChar = s[cnt];
	}
	return 0;
}
