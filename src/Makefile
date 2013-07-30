
include ../config.mk

SOURCES = \
  ThreadPool.cpp \
  ThreadPool.h \
  PoolThread.cpp \
  PoolThread.h \
  Thread.cpp \
  Thread.h \
  TaskPool.cpp \
  TaskPool.h \
  Task.cpp \
  Task.h \
  Mutex.cpp \
  Mutex.h \
  Timer.cpp \
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

%.o:	%.cpp
	$(CC) -c $(CFLAGS) $< -o $@ 

all: libthrpool.a

libthrpool.a:	$(SOURCES) $(OBJECTS)
	$(AR) ../libthrpool.a $(OBJECTS)
	$(RANLIB) ../libthrpool.a

clean:
	rm -rf *.o *~ libthrpool.a

