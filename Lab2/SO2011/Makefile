TARGET  := so2011
#WARN   := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
WARN    := -Wall
INCLUDE := -isystem /usr/src/linux/include -I.
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE}
CC      := gcc
	
${TARGET}.o: ${TARGET}.c

.PHONY: clean

clean:
	rm -rf ${TARGET}.o

