CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
SRCDIR=src/
objects=$(addprefix $(SRCDIR),\
		ax25decode.o addressing.o control.o info.o pid.o fcs.o\
		common_functions.o file_io.o\
		)


ax25encode : $(objects) 
	$(CC) -o ax25decode $(objects) $(CFLAGS) $(LDFLAGS)

.PHONY : clean
clean :
	rm $(objects) ax25decode


