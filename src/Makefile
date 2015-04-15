ROOT=..

.PHONY=clean

CC=gcc
CFLAGS= -fPIC -Wall -Wextra
LDFLAGS= -shared

SOURCES=thread.c
HEADERS=thread.h
OBJECTS=$(SOURCES:.c=.o)

LIBDIR=../lib
TARGET=$(LIBDIR)/thread.so

all: $(TARGET)

clean:
	rm -rf $(OBJECTS) $(TARGET)
	touch $(ROOT)/lib/thread.so 


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
