
DIR=
CCFLAGS= -Wall -g -pthread

ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	CC=aarch64-none-linux-gnu-g++
else
	CC=g++
endif

.phony: all
all:
	$(CC) $(CCFLAGS) aesdsocket.cpp -o aesdsocket -lstdc++

.phony: clean
clean:
	rm -f $(DIR)aesdsocket