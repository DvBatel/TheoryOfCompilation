.PHONY: all clean

CC = g++
CFLAGS = -std=c++17

all: clean
	flex scanner.lex
	bison -Wall -d parser.y
	$(CC) $(CFLAGS) -o hw2 *.c *.cpp
clean:
	rm -f lex.yy.* parser.tab.* hw2
