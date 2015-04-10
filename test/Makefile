ROOT=../

CC= gcc
CFLAGS= -g -Wall -std=c99 -I$(ROOT)/src -I.
LDFLAGS= -L$(ROOT)/lib
MAKE= /usr/bin/make

BIN=	01-main \
	02-switch \
	11-join \
	12-join-main \
	21-create-many \
	22-create-many-recursive \
	23-create-many-once \
	31-switch-many \
	32-switch-many-join \
	51-fibonacci

SUBDIR= test

all: $(BIN)
	@echo "Binaries created !"

%: %.c
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.c


clean:
	@rm -f $(BIN) *~ *.o
	@echo "Directory cleaned!"
