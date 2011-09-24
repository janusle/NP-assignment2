#ifndef CONFPROCESS_H
#define CONFPROCESS_H

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include "error.h"
#include "http.h"

#define OPTION 0
#define VALUE 1
#define ON 1
#define OFF 0
#define TYPESIZ 50
#define TYPNUM 200
#define PREFIX 5

#define ARRAYLEN 2
#define LEN 150
#define LINENUM 200


void explode( char* line, char* array[ARRAYLEN] );

void init_config( char* filename, char config[][CONFSIZE], 
                  contenttyp* type[TYPNUM] );



#endif
