
DIR=
CCFLAGS ?= -Wall -g 
LDFLAGS ?= -pthread -lstdc++
TARGET ?= aesdsocket

ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	CC ?= aarch64-none-linux-gnu-g++
else
	CC ?= g++
endif

.phony: all
all:
	$(CC) $(CCFLAGS) aesdsocket.cpp -o $(TARGET) $(LDFLAGS)

.phony: clean
clean:
	rm -f $(DIR)aesdsocket