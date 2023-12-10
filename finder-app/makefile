
DIR=

ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	CC=aarch64-none-linux-gnu-gcc
else
	CC=gcc
endif


.phony:	clean
.phony:	all

all:
	$(CC) -o $(DIR)writer $(DIR)writer.c

clean:
	rm $(DIR)writer 
