/*
 * retarget.h
 *
 *  Created on: 03.07.2015
 *      Author: Vaio
 */

#ifndef INCLUDES_RETARGET_H_
#define INCLUDES_RETARGET_H_

#include <stdio.h>

#define fputc		fputc_overriden
#define fputs		fputs_overriden

extern int fputc(int ch, FILE *f);

extern int fputs(const char *s, FILE *f);

#endif /* INCLUDES_RETARGET_H_ */
