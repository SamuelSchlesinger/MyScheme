#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "lexer.h"
#include "parser.h"
#include "evaluation.h"
#include <unistd.h>
#ifndef MAX_TOKEN_LENGTH
#define MAX_TOKEN_LENGTH 100
#endif

static void init() {
    t = (List) malloc(sizeof(struct cons_cell));
    t->content = "#t";
    f = (List) malloc(sizeof(struct cons_cell));
    f->content = "()"; 
    environment = (List) malloc(sizeof(struct cons_cell));  
    environment->content = "()"; 
    initialize_SCHEME_HEAP();
}

int main(int argc, char * argv[])
{

    init();

    printf("\nScheme!\n\n");

    int garbage = 0;

    if (argc > 1) {
        if (!strcmp(argv[1], "ng")) {
            garbage = 1;
        }
    }

    while(1)
    {
        printf("s_exp >>  ");   
        
        List test = S_Expression();
        printf("\n");
        List result = eval(test, environment, 0);
        printf("value >>  ");
        printList(result); 
        printf("\n\n");
        if (!garbage) {
            garbageCollect();
        }
    }

    printf("\n");

    return EXIT_SUCCESS;
}
