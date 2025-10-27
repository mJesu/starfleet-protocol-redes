CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
INCLUDES = -Isrc

SRCDIR = src
INCLUDEDIR = src
OBJDIR = obj
BINDIR = bin

SERVER_SRCS = $(SRCDIR)/servidor.c $(SRCDIR)/common.c $(SRCDIR)/jogo.c
CLIENT_SRCS = $(SRCDIR)/cliente.c $(SRCDIR)/common.c $(SRCDIR)/jogo.c

SERVER_OBJS = $(OBJDIR)/servidor.o $(OBJDIR)/common.o $(OBJDIR)/jogo.o
CLIENT_OBJS = $(OBJDIR)/cliente.o $(OBJDIR)/common.o $(OBJDIR)/jogo.o

SERVER_EXEC = $(BINDIR)/server
CLIENT_EXEC = $(BINDIR)/client

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJS)

$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJS)

$(OBJDIR)/servidor.o: $(SRCDIR)/servidor.c $(INCLUDEDIR)/protocol.h $(INCLUDEDIR)/common.h $(INCLUDEDIR)/jogo.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/cliente.o: $(SRCDIR)/cliente.c $(INCLUDEDIR)/protocol.h $(INCLUDEDIR)/common.h $(INCLUDEDIR)/jogo.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/common.o: $(SRCDIR)/common.c $(INCLUDEDIR)/common.h $(INCLUDEDIR)/protocol.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/jogo.o: $(SRCDIR)/jogo.c $(INCLUDEDIR)/jogo.h $(INCLUDEDIR)/protocol.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(SERVER_EXEC) $(CLIENT_EXEC)