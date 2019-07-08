INCLUDE := -isystem /Lab2/semSO -I.
WARN    := -Wall -Werror -O1
CFLAGS  := ${WARN} ${INCLUDE}
CC      := gcc

all : p1 p2 p3
	
p1 : p1.c semSO.o
p2 : p2.c semSO.o
p3 : p3.c semSO.o
semSO.o : semSO.c

.PHONY: clean

clean:
	rm -rf p1 p2 p3 semSO.o