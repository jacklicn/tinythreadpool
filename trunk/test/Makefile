
include ../config.mk

%.o:	%.cpp
	$(CC) -c $(CFLAGS) -I../src $< -o $@ 

all: thrtest

thrtest: ../libthrpool.a test.cpp test.o
	$(CC) $(CFLAGS) -o thrtest test.o ../libthrpool.a $(LFLAGS)

clean:
	rm -rf *.o *~ thrtest


