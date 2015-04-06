CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
SRCDIR=src/
objects=$(addprefix $(SRCDIR),\
		ax25encode.o addressing.o control.o info.o pid.o\
		)


ax25encode : $(objects) 
	$(CC) -o ax25encode $(objects) $(CFLAGS) $(LDFLAGS)

.PHONY : clean
clean :
	rm $(objects) ax25encode


