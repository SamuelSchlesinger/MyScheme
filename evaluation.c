#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lexer.h"
#include "parser.h"
#include "garbage.h"
#include "evaluation.h"

List t, f;
int initialized = 0;


List length(List);
List append(const List, const List); // functional
List null(List); // functional 
List assoc(const List, List); // functional 
List car(List); // functional 
List cons(List, List); // functional
List equal(List, List); // functional
List cond(List, List, int); 
List isSymbol(List); // functional
List quote(List); // functional 
List arithmeticCompare(List, List, int);
List add(List, List); 
List sub(List, List);
List nth(List, int n);
List and(List, List);
List or(List, List);
List not(List);
List list(List, List);
List mult(List, List);
List divide(List, List);
List mod(List, List);
List define(List, List, List); // functional
List number(List);
List copy(List); // functional
List isList(List);
List isFunction(List);
List let(List, List, List, int);
List flatten(List, List);
double atod(char* str);
char* dtoa(double str);

List system_call(List a, List env) {
    if (a->content != NULL) {
        int len = strlen(a->content);
        if (*a->content == '\"') {
            char* content_ptr = a->content + 1;
            char call[len - 2];
            char* call_ptr = call;
            while (*content_ptr != '\"') {
                *call_ptr++ = *content_ptr++;
            }
            system(call);
            printf("\n");     
            return createSymbol(call);
        } else {
            system(a->content);
            printf("\n");
            return createSymbol(a->content);
        }
    } else 
        return createSymbol("No call given");
}

List arithmeticCompare(List left, List right, int greater_than) {
    double left_d = atod(left->content); 
    double right_d = atod(right->content);
    if (greater_than == 1) {
        if (left_d > right_d) return t;
        else return f;
    } if (greater_than == 2) {
        if (left_d == right_d) return t;
        else return f;
    } else {
        if (right_d > left_d) return t;
        else return f;
    }
}

List last(List a) {
    for (; a->cdr!= NULL; a = a->cdr);
    return car(a);
}

List add(List a, List env) {
    double total = 0;
    for (; a != NULL; a = a->cdr) {
        List current = eval(car(a), env, 1);
        if (current->content != NULL) { 
            total += atod(current->content);
        }
    }   
    char* result_ptr = (char*) malloc(sizeof(char) * 20);
    sprintf(result_ptr, "%f", total);
    return createSymbol(result_ptr);  
}

List mod(List a, List b) {
    int a_d = (int) atod(a->content);
    int b_d = (int) atod(b->content);
    int answer = a_d%b_d;
    char* r = (char*) malloc(sizeof(char) * 20);
    sprintf(r, "%d", answer);
    return createSymbol(r);
}

List and(List a, List env) {
    for (; a != NULL; a = a->cdr) {
        List predicate = eval(car(a), env, 1);
        if (isFalse(predicate)) return f;
    } return t;
}

List or(List a, List env) {
    for (; a != NULL; a = a->cdr) {
        List predicate = eval(car(a), env, 1);
        if (isTrue(predicate)) return t;
    } return f;
}

List not(List a) {
    if (isFalse(a)) return t;
    return f;
}

List mult(List a, List env) {
    List first = eval(car(a), env, 1);
    double total;
    if (first->content != NULL) {
        total = atod(first->content);
    }
    a = a->cdr;
    for (; a != NULL; a = a->cdr) {
        List current = eval(car(a), env, 1);
        if (current->content != NULL) {
            total *= atod(current->content);
        }
    } 
    char* result_ptr = (char*) malloc(sizeof(char) * 20);
    sprintf(result_ptr, "%f", total);
    return createSymbol(result_ptr);
}

List divide(List a, List env) {
    List first = eval(car(a), env, 1);
    double total;
    if (first->content != NULL) {
        total = atod(first->content);
    }
    a = a->cdr;
    for (; a != NULL; a = a->cdr) {
        List current = eval(car(a), env, 1);
        if (current->content != NULL) {
            total /= atod(current->content);
        }
    }
    char* result_content = (char*) malloc(sizeof(char) * 20);
    sprintf(result_content, "%f", total);
    return createSymbol(result_content);
}

List sub(List a, List env) {
    List first = eval(car(a), env, 1);
    double total;
    if (first->content != NULL) {
        total = atod(first->content);
    }
    a = a->cdr;
    for (; a != NULL; a = a->cdr) {
        List current = eval(car(a), env, 1);
        if (current->content != NULL) {
            total -= atod(current->content);
        }
    }
    char* result_content = (char*) malloc(sizeof(char) * 20);
    sprintf(result_content, "%f", total);
    return createSymbol(result_content);
}

double atod(char* str) {
    int negative = 0;

    if (*str == '-') {
        negative = 1;
        str++;
    }

    double d = 0;
    while (*str != '.' && *str != '\0') {
        d*=10;
        d+=*str - '0';
        str++;
    }
    if (*str == '.') {
        str++;
        double level = 0.1;
        while (*str != '\0') {
            d += (*str - '0') * level;
            level /= 10;
            str++;
        }
    }

    if (negative) return -d;
    return d;
}

int isFalse(List l) {
    return (l == f);
}

int isTrue(List l) {
    return (l == t);
}

List isFunction(List a) {
    if (isSymbol(car(a)) == t) {
        if (a->car == NULL) return f;
        if (a->car->content == NULL) return f;
        if (!strcmp(a->car->content, "lambda")) return t;
    }
    return f;
}

List append_help(List a, List b) {
    if (a == NULL) {
        return b;
    }
    else return cons(car(a), append_help(a->cdr, b));
}

List isList(List a) {
    if (isSymbol(a) == f) return t;
    else return f;
}

List copy(List a) {
    if (a == NULL) return NULL;
    if (a->content != NULL) {

        return createSymbol(a->content);
    }
    else {
        List toReturn = createList();
        toReturn->car = copy(a->car);
        toReturn->cdr = copy(a->cdr);
        return toReturn;
    }
}

List define (List id, List expression, List env) {
    List definition = createList();
    definition->cdr = createList();
    // case for variables
    if (id->content != NULL) {
        definition->car = copy(id);
        definition->cdr->car = copy(eval(expression, env, 1));
    }
    // case for functions 
    else {
        definition->car = copy(car(id));
        List innards = createList();
        innards->car = createSymbol("lambda");
        innards->cdr = createList();
        List innards2 = innards->cdr;
        if (cdr(id) != f) {
            innards2->car = copy(cdr(id));
        } else {
            innards2->car = f;
        }
        innards2->cdr = createList();
        innards2->cdr->car = copy(expression);
        definition->cdr->car = copy(innards);
    } 
    return cons(definition, env);
}

List append(List a, List b) {
    if (isFalse(a)) return b;
    if (isFalse(b)) return a;
    else {
        return append_help(copy(a), b);
    }
} 

List null(List a) {
    if (isFalse(a)) {
        return t;
    }
    else return f;
}

// returns value associated with symbol in the passed association list
List assoc(const List symbol, const List association_list) {
    if (association_list->car != NULL) {
        if (association_list == f) {
            return f;
        } else if (!strcmp(symbol->content, association_list->car->car->content)) {
            return car(association_list);
        } else {
            return assoc(symbol, cdr(association_list));
        }
    } else return f;
}


// ( ( predicate expression ) ( predicate expression) ... (else expression) )
List cond(List a, List env, int level) {  
    List conditional;
    List predicate; 
    List preEvalPredicate;
    List expression;
    int i = 0;
    while (!isFalse(a)) {
        conditional = car(a);
        preEvalPredicate = car(conditional);
        expression = copy(car(cdr(conditional)));
        if (preEvalPredicate->content != NULL) {
            if (!strcmp(preEvalPredicate->content, "else")) {
                return eval(expression, env, level + 1);
            }
        }
        predicate = eval(preEvalPredicate, env, level + 1);
        if (isTrue(predicate)) {
            return eval(expression, env, level + 1);
        } 
        a = cdr(a);
        i++;
    }
    return f;
}

List if_form(List test_expr, List then_expr, List else_expr, List env, int level) {
    if (eval(test_expr, env, level + 1) == t) {
        return eval(then_expr, env, level + 1);
    } else return eval(else_expr, env, level + 1);
}

static int equal_helper(List a, List b) {
    if (a->content != NULL && b->content != NULL) {
        if (!strcmp(a->content, "#f") && !strcmp(b->content, "()")) {
            return 1;
        }
        if (!strcmp(a->content, b->content)) {
            return 1;         
        } else return 0;
    } else {
        int carsMatch;
        int cdrsMatch;
        if (car(a) != NULL) {
            if (car(b) != NULL) {
                carsMatch = equal_helper(car(a), car(b));
            } else carsMatch = 0;
        } else if (car(b) != NULL) {
            if (car(a) != NULL) {
                carsMatch = equal_helper(car(a), car(b));
            } else carsMatch = 0;
        } else carsMatch = 1;
        if (cdr(a) != NULL) {
            if (cdr(b) != NULL) {
                cdrsMatch = equal_helper(cdr(a), cdr(b));
            } else cdrsMatch = 0;
        } else if (cdr(b) != NULL) {
            if (cdr(a) != NULL) {
                cdrsMatch = equal_helper(cdr(a), cdr(b));
            } else cdrsMatch = 0;
        } else cdrsMatch = 1;
        return (cdrsMatch && carsMatch);
    }
}

List equal(List a, List b) {
    if (equal_helper(a, b)) 
        return t;
    else 
        return f;
}

/* Returns the car of a Cons Cell */
List car(List l) {
    if (l == NULL) return f;
    if (l->car == NULL) return f;
    return l->car;
}

/* Returns the cdr of a Cons Cell */
/*List cdr(List l) {
    if (l->cdr == NULL) {
        return f;
    } else {
        List l2 = l;
        if (l2->car == NULL) printf("carNULL");
        if (l2->cdr == NULL) printf("cdrNULL");
        if (l2->content == NULL) printf("contentNULL");
    }
    return l->cdr;
}*/

List quote(List l) {
    return l;
}

List list(List a, List env) {
    a = copy(a);
    List rem = a;
    for (; a != NULL; a = a->cdr) {
        a->car = eval(a->car, env, 1);
    }
    return rem;
}

List cons(List push, List to) {
    List l = malloc(sizeof(struct cons_cell));
    if (isFalse(to)) {
            l->car = push;
            l->cdr = NULL;
    } else {
        l->car = push;
        l->cdr = to;
    }
    return l;
}

List isSymbol(List l) {
    if (l->content != NULL) {
        return t;
    } else {
        return f;
    }
}

int c_isFunction(List l) {
    if (l->content != NULL) return 0;
    if (l->car->content != NULL) {
        if (!strcmp(l->car->content, "lambda")) {
            return 1;
        }
    } return 0;
}

int c_length(List a) {
    int i = 0; 
    for (; a != NULL; a = a->cdr) {
        i++;
    }
    return i;
}

List length(List a) {
    int i = c_length(a);
    char* n = (char*) malloc(sizeof(char) * 20);
    sprintf(n, "%d", i);
    return createSymbol(n);
}

List let(List definition, List expression, List env, int level) {
    env = define(definition->car, definition->cdr->car, env); 
    return eval(expression, env, level + 1); 
}

List lambdaEvaluation(List lambda, List toEvaluate, List env, int level) {
    List parameter_names = car(cdr(lambda));
    List arguments = cdr(toEvaluate);
    int more = 1;
    int error = 0;
    int len = c_length(parameter_names);
    while (len--) {
        List symbol = copy(car(parameter_names));
        List value = copy(car(arguments));
        env = define(symbol, value, env);
        parameter_names = cdr(parameter_names);
        arguments = cdr(arguments);
        if (parameter_names == f) {
            more = 0;
        } else if (arguments == f) {
            error = 1;
            more = 0;
        }
    }
    List lambdaBody = copy(car(cdr(cdr(lambda))));
    //printf("environment at level %d: ", level); printList(env); printf("\n");
    return eval(lambdaBody, env, level + 1);
}

List nth(List a, int n) {
    for (; n != 0; n--) {
        a = cdr(a);
    } return car(a);
}

List number(List a) {
    if (a->content == NULL) {
        return f;
    }
    char* str = a->content;
    int decimal = 0;
    if (*str == '-') str++;
    while (*str) {
        if (!((*str >= '0') && (*str <= '9')) && (*str != '.')) {
            return f;
        }
        if (*str == '.') {
            if (decimal) {
                return f;
            }
        str++;
        decimal = 1;
        } else str++;
    }
    return t;
}

List flatten(List l, List env) {
    if (isFalse(l)) {
        return f;
    } else if (l->content != NULL) {
        List new_l = createList();
        new_l->car = l;
        return new_l;
    } else {
        return append(flatten(car(l), env), flatten(cdr(l), env));
    }
}

List eval(List toEvaluate, List env, int level) {
    char* func; 

    if (c_isFunction(toEvaluate)) {
        return toEvaluate;
    }

    // case for symbols
    if (toEvaluate->content != NULL) { 
        if (toEvaluate->content[0] == '\"') return toEvaluate;
        // case for false
        if (isFalse(toEvaluate)) {
            return f;
        }
        // case for true 
        else if (toEvaluate == t || !strcmp(toEvaluate->content, "#t")) {
            return t;
        }
        // case for variables 
        List pair = assoc(toEvaluate, env);
        if (pair != f) {
            toEvaluate = copy(car(cdr(pair)));
            return toEvaluate;
        }
        // case for numbers
        else {
            char* str = toEvaluate->content;
            int decimal = 0;
            if (*str == '-') ++str;
            if (!*str) return 0;
            while (*str) {
                if (!((*str >= '0') && (*str <= '9')) && (*str != '.')) {
                    char* err = (char*) malloc(strlen(str) + 32);
                    scanf(err, "err#2: %s is not a defined symbol\0", str);
                    return createSymbol(err);
                }
                if (*str == '.') {
                    if (decimal) {
                        char* err = (char*) malloc(strlen(str) + 32);
                        scanf(err, "err#2: %s is not a defined symbol\0", str);
                        return createSymbol(err);
                    } 
                    str++; 
                    decimal = 1;
                }
                else str++;
            } return toEvaluate;
        }
    }

    // case for user defined functions
    List var;
    if ((var = assoc(toEvaluate->car, env)) != f) {
        List lambda = car(cdr(var));
        if (c_isFunction(lambda)) 
            return lambdaEvaluation(lambda, toEvaluate, env, level);
    } else if (c_isFunction(toEvaluate->car)) {
        List lambda = car(toEvaluate);
        return lambdaEvaluation(lambda, toEvaluate, env, level);
    }

    func = toEvaluate->car->content; 
    if (!strcmp(func, "car")) {
       return car(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "cdr")) {
       return cdr(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "cons")) {
       return cons(eval(car(cdr(toEvaluate)), env, level + 1), eval(car(cdr(cdr(toEvaluate))), env, level + 1));
    } else if (!strcmp(func, "quote")) {
       return quote((car(cdr(toEvaluate))));
    } else if (!strcmp(func, "cond")) {
       return cond((cdr(toEvaluate)), env, level + 1);
    } else if (!strcmp(func, "if")) {
       return if_form(car(cdr(toEvaluate)), car(cdr(cdr(toEvaluate))), car(cdr(cdr(cdr(toEvaluate)))), env, level);
    } else if (!strcmp(func, "assoc")) {
       return assoc(eval((car(cdr(toEvaluate))), env, level + 1), eval((car(cdr(cdr(toEvaluate)))), env, level + 1)); 
    } else if (!strcmp(func, "symbol?")) {
       return isSymbol(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "eval")) {
       return eval(eval(car(cdr(toEvaluate)), env, level + 1), env, level + 1);
    } else if (!strcmp(func, "let")) {
       return let(car(cdr(toEvaluate)), car(cdr(cdr(toEvaluate))), env, level + 1);
    } else if (!strcmp(func, "null?")) {
       return null(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "equal?")) {
       return equal(eval(car(cdr(toEvaluate)), env, level + 1), eval(car(cdr(cdr(toEvaluate))), env, level + 1));
    } else if (!strcmp(func, "append")) {
       return append(eval(car(cdr(toEvaluate)), env, level + 1), eval(car(cdr(cdr(toEvaluate))), env, level + 1));
    } else if (!strcmp(func, "list?")) {
       return isList(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "flatten")) {
       return flatten(eval(car(cdr(toEvaluate)), env, level + 1), env);
    } else if (!strcmp(func, "function?")) {
       return isFunction(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "+")) {
       return add(cdr(toEvaluate), env);
    } else if (!strcmp(func, "*")) {
       return mult(cdr(toEvaluate), env);
    } else if (!strcmp(func, "/")) {
       return divide(cdr(toEvaluate), env);
    } else if (!strcmp(func, "-")) {
       return sub(cdr(toEvaluate), env);
    } else if (!strcmp(func, ">")) {
       return arithmeticCompare(eval(car(cdr(toEvaluate)), env, level + 1), eval(car(cdr(cdr(toEvaluate))), env, level + 1), 1);
    } else if (!strcmp(func, "<")) {
       return arithmeticCompare(eval(car(cdr(toEvaluate)), env, level + 1), eval(car(cdr(cdr(toEvaluate))), env, level + 1), 0);
    } else if (!strcmp(func, "=")) {
       return arithmeticCompare(eval(car(cdr(toEvaluate)), env, level + 1), eval(car(cdr(cdr(toEvaluate))), env, level + 1), 2);
    } else if(!strcmp(func, "remainder")) {
       return mod(eval(car(cdr(toEvaluate)), env, level + 1), eval(car(cdr(cdr(toEvaluate))), env, level + 1));
    } else if (!strcmp(func, "define")) {
       if (level == 0) {
           env = define(car(cdr(toEvaluate)), car(cdr(cdr(toEvaluate))), env);
           mark(env);
           environment = env;
       }
       return car(car((env)));
    } else if (!strcmp(func, "and")) {
       return and(cdr(toEvaluate), env);
    } else if (!strcmp(func, "or")) {
       return or(cdr(toEvaluate), env);
    } else if (!strcmp(func, "not")) {
       return not(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "number?")) {
       return number(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "last")) {
       return last(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "length")) { 
       return length(eval(car(cdr(toEvaluate)), env, level + 1));
    } else if (!strcmp(func, "list")) {
       return list(cdr(toEvaluate), env);
    } else if (!strcmp(func, "heap-size")) {
       return getN();
    } else if (!strcmp(func, "environment")) {
       return env;
    } else if (!strcmp(func, "clear") || !strcmp(func, "l")) {
       system("clear");
       return createSymbol("clear"); 
    } else if (!strcmp(func, "quit") || !strcmp(func, "exit")) {
       printf("Goodbye!\n\n");
       exit(0);
    } else if (!strcmp(func, "syscall")) {
       return system_call(eval(car(cdr(toEvaluate)), env, level + 1), env);
    } else {
       char* ptr = func;
       if (*ptr++ == 'c' && *ptr++ == 'a') {
           int n = 0;
           int i = 0;
           int len = strlen(func);
           for (; *(ptr+1) != 0; ptr++) {
               if (*ptr == 'd') n++; 
               else return createSymbol("WHY CAN'T YOU JUST ASK ME FOR THINGS I KNOW?"); 
           } if (*ptr == 'r') return nth(eval(car(cdr(toEvaluate)), env, level+1), n);
       } else {
           char* error = (char*) malloc(sizeof(char) * strlen(func) + 30);
           sprintf(error, "err#1: %s is an undefined function.", func);
           return createSymbol(error);
       }
    }
}
