#include "common.h"
#include "protocol.h"
#include "jogo.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void usage(int argc, char **argv) {
    if (argc > 0) {
        printf("Cliente de Batalha\n");
    }
    printf("Uso: %s <server IP> <server port>\n", argv[0]);
    printf("Exemplo: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != addrparse(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int client_socket;
    client_socket = socket(storage.ss_family, SOCK_STREAM, 0);
    if (client_socket == -1) {
        logexit("socket");
    }
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != connect(client_socket, addr, sizeof(storage))) {
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("Conectado ao servidor %s\n", addrstr);

    PlayerInfo *client_player = create_player();
    BattleMessage msg;

    while (1) {
        recv_message(client_socket, &msg);

        switch (msg.type) {
            case MSG_INIT: {
                print_msg(MSG_INIT);
                break;
            }

            case MSG_ACTION_REQ: {
                print_msg(MSG_ACTION_REQ);
                int acao;
                while (1) {
                    scanf("%d", &acao);
                    if (acao < 0 || acao > 4) {
                        printf("Erro na escolha! Escolha entre 0-4: ");

                        int c;
                        while ((c = getchar() != '\n' && c != EOF));
                        continue;
                    }
                    break;
                }

                msg.type = MSG_ACTION_RES;
                msg.client_action = acao;
                send_message(client_socket, &msg);
                break;
            }

            case MSG_BATTLE_RESULT: {
                client_player->hp = msg.client_hp;
                client_player->torpedos_usados = msg.client_torpedoes;
                client_player->escudos_usados = msg.client_shields;

                PlayerInfo server_player;
                server_player.hp = msg.server_hp;

                print_resultado_turno(client_player, &server_player, msg.client_action, msg.server_action);

                break;
            }
            case MSG_GAME_OVER: {
                PlayerInfo server_player;
                server_player.hp = msg.server_hp;
                int vitoria = (client_player->hp > 0 && msg.server_hp <= 0) ? 1 : (client_player->hp <= 0) ? -1 : 0;
                print_finalizacao(client_player, &server_player, vitoria);
                
                free(client_player);
                close(client_socket);
                exit(EXIT_SUCCESS);
                break;
            }
            case MSG_ESCAPE: {
                PlayerInfo server_player;
                server_player.hp = msg.server_hp;
                print_finalizacao(client_player, &server_player, 0);

                free(client_player);
                close(client_socket);
                exit(EXIT_SUCCESS);
                break;
            }
        }
    }
    close(client_socket);

    exit(EXIT_SUCCESS);
}
