#ifndef GARBAGE_UTIL
#define GARBAGE_UTIL
#include "parser.h"

/* Garbage Collection */
void initialize_SCHEME_HEAP();
void print_SCHEME_HEAP();
void garbageCollect();
void pushToHeap(List);
void mark(List);
List getN();

#endif
