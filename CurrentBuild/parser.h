#ifndef PARSER
#define PARSER
#include <stdlib.h>
#include "lexer.h"
#define MAX_TOKEN_SIZE 200

/* List type which will hold all data for Scheme expressions */
typedef struct cons_cell *List;

/* Universal Lists which are used for #t, #f and the environment */
List t, f, environment;

/* Prints List */
void printList(List toPrint);

/* Returns a List structure as parsed from input */
List S_Expression();

/* Returns an atom containing the given string */
List createList(char*);

/* CDR */
List cdr(List);

#endif
