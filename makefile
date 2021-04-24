CFLAGS=-std=c11 -g -O0 -static
SRCS  =$(wildcard *.c)
OBJS  =$(SRCS:.c=.o)

assembler: $(OBJS)
		$(CC) -o assembler $(OBJS) $(LDFLAGS)

$(OBJS): assembler.h

parse: parse.o
		$(CC) -o parse.o parse.c 
clean:
		rm -f assembler *.o *~ tmp*

.PHONY:	clean