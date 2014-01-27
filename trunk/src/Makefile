
include ../config.mk

SOURCES = \
  ThreadPool.cc \
  ThreadPool.h \
  PoolThread.cc \
  PoolThread.h \
  Thread.cc \
  Thread.h \
  TaskPool.cc \
  TaskPool.h \
  Task.cc \
  Task.h \
  Mutex.cc \
  Mutex.h \
  Timer.cc \
  Timer.h \
  TimeUnit.h 

OBJECTS = \
  ThreadPool.o \
  PoolThread.o \
  Thread.o \
  TaskPool.o \
  Task.o \
  Mutex.o \
  Timer.o 

%.o:	%.cc
	$(CC) -c $(CFLAGS) $< -o $@ 

all: libthrpool.a

libthrpool.a:	$(SOURCES) $(OBJECTS)
	$(AR) ../libthrpool.a $(OBJECTS)
	$(RANLIB) ../libthrpool.a

clean:
	rm -rf *.o *~ libthrpool.a

