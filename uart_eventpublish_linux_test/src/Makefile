


IDIR =../include
CC=gcc
CFLAGS=-I.

ODIR=obj
LDIR =../lib

LIBS=-lm

DEPS = ipc_message_publishqueue.h ipc_message_subscribequeue.h ipc.h ipc_thread.h common_ipc_enum.h

OBJ = uart_eventpublish.o ipc_message_publishqueue.o ipc_message_subscribequeue.o ipc.o ipc_thread.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)