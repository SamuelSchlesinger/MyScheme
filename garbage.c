#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "garbage.h"

struct cons_cell {
    struct cons_cell* car;
    struct cons_cell* cdr;
    char* content;
    int keep;
};

// in the spirit of Scheme, the heap is implemented as a linked list
typedef struct node {
    List L;
    struct node* next;
} *SCHEME_HEAP;

void initialize_SCHEME_HEAP();
void print_SCHEME_HEAP();
void mark(List);
int N;
int N_DELETED;
SCHEME_HEAP heap;
SCHEME_HEAP end;

List getN() {
    char* str = (char*) malloc(sizeof(char) * 20);
    sprintf(str, "%d", N);
    return createSymbol(str);
}

void printN_DELETED() {
    printf("%d\n", N_DELETED);
}

SCHEME_HEAP createEmptyNode() {
    SCHEME_HEAP sh = (SCHEME_HEAP) malloc(sizeof(struct node));
    sh->L = NULL;
    sh->next = NULL;
    return sh;
}

void initialize_SCHEME_HEAP() {
    N = 0;
    N_DELETED = 0;
    heap = createEmptyNode();
    heap->next = createEmptyNode();
    end = heap->next;
}

void print_SCHEME_HEAP() {
   struct node * ptr = heap->next;
   int i = 0;
   printf("HEAP SIZE: %d ", N);
   for (; ptr != end; ptr = ptr->next) {
       if (ptr != NULL) {
           i++; 
           printf("HEAP NODE #%d: ", i);
           if (ptr->L != NULL)
               printList(ptr->L);
           printf("\n");
       }
   }   
}

void pushToHeap(List l) {
   end->L = l;
   end->next = createEmptyNode();
   end = end->next;
   N++;
}

void garbageCollect() {    
   struct node * current = heap->next; 
   struct node * last = heap;
   mark(environment);
   int i = 0;
   for (; current != end;) {
       if (!current->L->keep) {
           free(current->L);
           last->next = current->next;
           struct node* p = current;
           current = current->next;
           N--;
           N_DELETED++;
           free(p);
           i++;
       } else {
           last = current;
           current = current->next;
       }
   }
}

void mark(List current) {
    current->keep = 1;
    if (current->car != NULL) 
        mark(current->car);      
    if (current->cdr != NULL)
        mark(current->cdr);
}
