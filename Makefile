CC=gcc


9cc: 9cc.c
	$(CC) 9cc.c -o 9cc -ggdb

test: 9cc
	./gcc -test
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*