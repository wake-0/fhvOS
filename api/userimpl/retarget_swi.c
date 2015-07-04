/*
 * retarget_swi.c
 *
 *  Created on: 03.07.2015
 *      Author: Nicolaj Hoess
 */

#include <stdio.h>
#include <string.h>
#include "../includes/system.h"
#include "../includes/retarget_swi.h"

FILE __stdout =     { 0 };
FILE __stdin =      { 1 };

int fputc_overriden(int ch, FILE *f)
{
    char tempch = ch;
    switch (tempch) {
    case '\n':
    	print("\r\n", 2);
    	break;
    default:
    	print(&tempch, 1);
    	break;
    }
    return ch;
}

int fputs_overriden(const char *s, FILE *f)
{
	int len = strlen(s);
	if (len > 0)
	{
		print(s, len);
	}
	return 0;
}
