#pragma once

#include "protocol.h"

typedef struct {
    int hp;
    int torpedos_usados;
    int escudos_usados;
    int opcao;
} PlayerInfo;

PlayerInfo* create_player();
const char* get_nome_acao(int acao);
void print_info_acoes();
void print_resultado_turno(PlayerInfo *client, PlayerInfo *server, int client_action, int server_action);
void print_finalizacao(PlayerInfo *client, PlayerInfo *server, int vitoria);
void print_msg(MessageType msg);
int process_turno(PlayerInfo *client, PlayerInfo *server);
void print_resultado_turno(PlayerInfo *client, PlayerInfo *server, int client_action, int server_action);