
include ../config.mk

%.o:	%.cc
	$(CC) -c $(CFLAGS) -I../src $< -o $@ 

all: thrtest

thrtest: ../libthrpool.a test.cc test.o
	$(CC) $(CFLAGS) -o thrtest test.o ../libthrpool.a $(LFLAGS)

clean:
	rm -rf *.o *~ thrtest


