C = g++
CFLAGS=-I./src/ -L./lib -lcoroutine -std=c++11


all: test testperform colib clean

test: test/test.cc  colib
	g++ test/test.cc -o bin/test $(CFLAGS)

testperform: test/testperform.cc colib
	g++ test/testperform.cc -o bin/test-perform $(CFLAGS)
	

colib: 
	$(CC) src/coroutine.cc -c -std=c++11
	ar rcs lib/libcoroutine.a coroutine.o 

.PHONY: clean clear

clean:
	rm *.o -rf

clear:
	rm *.o -rf
	rm *.a -rf
	rm bin/* -r
