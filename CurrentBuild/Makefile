#To run, put this file together with lexer.h, and lexer.c
#in the same directory. Run "make". Then the executable
#is "schemer," which just takes a line of input and
#breaks it up into tokens.


scheme: lexer.o parser.o evaluation.o main.o 
	gcc -o scheme main.o evaluation.o parser.o lexer.o

evaluation.o: evaluation.c
	gcc -c evaluation.c

parser.o: parser.c 
	gcc -c parser.c

lexer.o: lexer.c
	gcc -c lexer.c

main.o: main.c
	gcc -c main.c

clean:
	rm -f *~ *.o *.a

#^^^^^^This space must be a TAB!!.

