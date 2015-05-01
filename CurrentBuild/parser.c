#include <stdio.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"


/**************************************************************************
* Recursive descent parser for the following EBNF notation:
* <S_Expression> -> ({S_Expression}) | #t | #f | <symbol>
* <symbol> -> any string of non-whitespace characters that does not begin
* with "(", ")", "#", or "'" (the single quote) and that does not contain
* "(" or ")"
***************************************************************************/
/* Holds the current token during parsing */
static char token[MAX_TOKEN_SIZE];

/* Recursively called helper function for main parsing function */
static List S_Exp(int LEVEL);

static void tab(int);

/* Main parsing function */
List S_Expression();

/* Cons Cell structure which all lists consist of 
   If content != NULL then it is a symbol node
*/

typedef struct cons_cell {
    List car;
    List cdr;
    char* content;
    int keep; 
} *List;

void mark(List);

typedef struct node {
    List L;
    struct node* next;
} *SCHEME_HEAP;

static SCHEME_HEAP createEmptyNode() {
    SCHEME_HEAP n = (SCHEME_HEAP) malloc(sizeof(struct node));
    n->L = NULL;
    n->next = NULL;
    return n;
}

static SCHEME_HEAP heap;
static SCHEME_HEAP end;

void initialize_SCHEME_HEAP() {
   heap = createEmptyNode();
   end = heap; 
}

void pushToHeap(List l) {
   end->L = l;
   end->next = createEmptyNode();
   end = end->next;
}

void garbageCollect() {    

}

void mark(List current) {
    if (current != NULL) {
        current->keep = 1;
        mark(current->car);
        mark(current->cdr);
    }
}

List createSymbol(char* x) {
    if (!strcmp (x, "#t")) {
        return t;
    } else if (!strcmp(x, "#f") || !strcmp(x, "()")) {
        return f;
    } 
    List l = (List) malloc(sizeof(struct cons_cell));
    l->car = NULL; l->cdr = NULL;
    l->content = x;
    pushToHeap(l);
    return l;
}

List createList() {
    List l = (List) malloc(sizeof(struct cons_cell));
    l->car = NULL;
    l->cdr = NULL;
    l->content = NULL;
    pushToHeap(l);
    return l;
}

List cdr(List a) {
    if (a == NULL) return f;
    if (a->cdr == NULL) return f;
    else return a->cdr;
}

List S_Expression() {
    startTokens(MAX_TOKEN_SIZE);
    strcpy(token, getToken());
    return S_Exp(0);
}


void printListHelp(List l, int level) {
    if (l->content != NULL) { 
        printf(" %s ", l->content);
    } else {
        if (l->car->content != NULL) {
            if (!strcmp(l->car->content, "quote")) {
    //            tab(level);
                printf("\'");
                printListHelp(l->cdr->car, level + 1);
                return;
            }
        } 
        printf("(");
        if (l->cdr == NULL)   
            printListHelp(l->car, level + 1);
        else 
            printListHelp(l->car, level + 1);
        while (l->cdr != NULL) {
            l = l->cdr;
            if (l->cdr == NULL) {
                if (l->car != NULL) 
                    printListHelp(l->car, level+1);   
            }
            else {
                if (l->car != NULL)
                    printListHelp(l->car, level+1);
            }
        }
        printf(")");
    }
}


void printList(List l) {
    printListHelp(l, 0);
}

static void tab(int LEVEL)
{
    int tabs;
    for (tabs = 0; tabs < LEVEL; tabs++)
    {
        printf("  ");
    }
}

/* Puts a new token into the token string */
void newToken()
{
    strcpy(token, getToken());
}

/* Helper function: RETURNS ALL LISTS */
static List S_Exp(int LEVEL)
{
    //tab(LEVEL);
    //printf("S_Expression\n");
    // if token begins another Scheme expression
    if (!strcmp(token, "("))
    {
        //tab(LEVEL); 
        //printf("%s\n", token);
        newToken();
        List toReturn = (List) malloc(sizeof(struct cons_cell));
        toReturn->content = NULL;
        toReturn->car = S_Exp(LEVEL+1);
        toReturn->cdr = (List) malloc(sizeof(struct cons_cell));
        toReturn->content = NULL;
        List current = toReturn->cdr;
        List last = toReturn;
        int things = 0;
        while (strcmp(token, ")"))
        {
             things = 1;
             current->content = NULL;
             current->car = S_Exp(LEVEL+1);
             current->cdr = (List) malloc(sizeof(struct cons_cell));
             last = current;
             current = current->cdr;
        }
        last->cdr = NULL;
        if (LEVEL)
        {
            newToken();
        }
        //tab(LEVEL);
        //printf(")\n");
        return toReturn;  
    }
    else
    {
        if (!strcmp(token, "\'")) {
            newToken();
            List toReturn = (List) malloc(sizeof(struct cons_cell));
            toReturn->car = (List) malloc(sizeof(struct cons_cell));
            toReturn->car->content = "quote";
            //tab(LEVEL);
            //printf("  quote\n");
            toReturn->cdr = (List) malloc(sizeof(struct cons_cell));
            toReturn->cdr->car = S_Exp(LEVEL);
            return toReturn;
        }
        if (!strcmp(token, "#t"))
        {
            //tab(LEVEL);
            //printf("  %s\n", token);
            if (LEVEL)
                newToken();
            return t;
        }
        if (!strcmp(token, "#f"))
        {
            //tab(LEVEL);
            //printf("  %s\n", token);
            if (LEVEL)
                newToken();
            return f;
        }
        else
        {
            //tab(LEVEL);
            int sharp_f = 0;
            if (!strcmp(token, "()")) sharp_f = 1;
            List toReturn = (List) malloc(sizeof(struct cons_cell));
            toReturn->content = (char*) malloc(sizeof(strlen(token) + 1));
            strcpy(toReturn->content, token);
            //printf("  %s\n", token);
            if (LEVEL)
                newToken();
            if (sharp_f) return f;
            return toReturn;
        }
    }
}

