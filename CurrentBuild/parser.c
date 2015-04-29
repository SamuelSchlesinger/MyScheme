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

/* Main parsing function */
List S_Expression();

/* Cons Cell structure which all lists consist of 
   If content != NULL then it is a symbol node
*/

typedef struct cons_cell {
    List car;
    List cdr;
    char* content;
} *List;


List createList(char* x) {
    List l = (List) malloc(sizeof(struct cons_cell));
    l->car = NULL; l->cdr = NULL;
    l->content = x;
    return l;
}

List cdr(List a) {
    if (a->cdr == NULL) return createList("#f");
    else return a->cdr;
}

List S_Expression() {
    startTokens(MAX_TOKEN_SIZE);
    strcpy(token, getToken());
    return S_Exp(0);
}


void printListHelp(List l, int state) {
    if (l->content != NULL) { 
        if (!state) 
            printf("%s ", l->content);
        else 
            printf("%s", l->content);
    } else {
        if (l->car->content != NULL) {
            if (!strcmp(l->car->content, "quote")) {
                printf("\'");
                printListHelp(l->cdr->car, 0);
                return;
            }
        } 
        printf("(");
        if (l->cdr == NULL)   
            printListHelp(l->car, 1);
        else 
            printListHelp(l->car, 0);
        while (l->cdr != NULL) {
            l = l->cdr;
            if (l->cdr == NULL) {
                if (l->car != NULL) 
                    printListHelp(l->car, 1);   
            }
            else {
                if (l->car != NULL)
                    printListHelp(l->car, 0);
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
        if (!things) {
            toReturn->cdr = NULL;
        } 
        
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
            List toReturn = (List) malloc(sizeof(struct cons_cell));
            toReturn->content = (char*) malloc(strlen(token) + 1);
            strcpy(toReturn->content, token);
            //printf("  %s\n", token);
            if (LEVEL)
                newToken();
            return toReturn;
        }
        if (!strcmp(token, "#f"))
        {
            //tab(LEVEL);
            List toReturn = (List) malloc(sizeof(struct cons_cell)); 
            toReturn->content = (char*) malloc(strlen(token) + 1);
            strcpy(toReturn->content, token);
            //printf("  %s\n", token);
            if (LEVEL)
                newToken();
            return toReturn;
        }
        else
        {
            //tab(LEVEL);
            List toReturn = (List) malloc(sizeof(struct cons_cell));
            toReturn->content = (char*) malloc(sizeof(strlen(token) + 1));
            strcpy(toReturn->content, token);
            //printf("  %s\n", token);
            if (LEVEL)
                newToken();
            return toReturn;
        }
    }
}

