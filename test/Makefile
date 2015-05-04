ROOT=..

CC= gcc
CFLAGS= -g -Wall -std=c99 -I$(ROOT)/src -I.
LDFLAGS= -L$(ROOT)/lib  

BIN=	01-main \
	02-switch \
	11-join \
	12-join-main \
	21-create-many \
	22-create-many-recursive \
	23-create-many-once \
	31-switch-many \
	32-switch-many-join \
	51-fibonacci \
	61-stack-overflow \
	62-stack-overflow \
	profiling


all: $(BIN)
	@echo "Binaries created !"

%: %.c
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.c

01-main: ../src/thread.o
02-switch: ../src/thread.o
11-join: ../src/thread.o
12-join-main: ../src/thread.o
21-create-many: ../src/thread.o
22-create-many-recursive: ../src/thread.o
23-create-many-once: ../src/thread.o
31-switch-many: ../src/thread.o
32-switch-many-join: ../src/thread.o
51-fibonacci: ../src/thread.o
61-stack-overflow: ../src/thread.o
62-stack-overflow: ../src/thread.o
profiling: ../src/thread.o

clean:
	@rm -f $(BIN) *~ *.o
	@echo "Directory cleaned!"
