#pragma once

#include "protocol.h"

#include <stdlib.h>
#include <arpa/inet.h>

void logexit(const char *msg);

void send_message(int csock, BattleMessage *msg);

void recv_message(int csock, BattleMessage *msg);

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr, struct sockaddr_storage *storage);
