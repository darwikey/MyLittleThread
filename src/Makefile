ROOT=..

.PHONY=clean

CC=gcc
CFLAGS= -fPIC -Wall -Wextra -D STACK_SIZE=4096 -D THREAD_PREEMPT -D THREAD_PREEMPT_INTERVAL=10000
LDFLAGS= -shared

SOURCES=thread.c
HEADERS=thread.h
OBJECTS=$(SOURCES:.c=.o)

LIBDIR=../lib
TARGET=$(LIBDIR)/thread.a

all: $(TARGET)

clean:
	rm -rf $(OBJECTS) $(TARGET)


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
