#ifndef PARSER
#define PARSER
#include <stdlib.h>
#include "lexer.h"
#define MAX_TOKEN_SIZE 200

/* List type which will hold all data for Scheme expressions */
typedef struct cons_cell *List;

/* Universal Lists which are used for #t, #f and the environment */
List t, f, environment;

/* Heap utils */
typedef struct node *SCHEME_HEAP;
void initialize_SCHEME_HEAP();

/* Prints List */
void printList(List toPrint);

void garbageCollect();

/* Returns a List structure as parsed from input */
List S_Expression();

/* Returns an atom containing the given string */
List createSymbol(char*);

List createList();

/* CDR */
List cdr(List);

#endif
