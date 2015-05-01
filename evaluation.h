#ifndef EVAL_UTILS
#define EVAL_UTILS
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

typedef struct cons_cell {
    List car;
    List cdr;
    char* content;
    int keep;
} *List;

/* Prints List */
void printList(List toPrint);

/* Evaluates a List, returning a List */
List eval(List toEvaluate, List environment, int level);

#endif
