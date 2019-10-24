CC=gcc
CFLAGS=-Wall -std=c++11 -g
SRCS=$(wildcard *.cc)
OBJS=$(SRCS:.cc=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(CFLAGS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./9cc -test
	./test.sh

clean:
	rm -f 9cc *.o *~ 
