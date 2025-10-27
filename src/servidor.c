#include "common.h"
#include "protocol.h"
#include "jogo.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    if (argc > 0) {
        printf("Servidor de Batalha\n");
    }
    printf("Uso: %s <v4|v6> <server port>\n", argv[0]);
    printf("Exemplo: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

struct client_data {
    int csock;
    struct sockaddr_storage storage;
};

void * client_thread(void *data) {
    struct client_data *cdata = (struct client_data *)data;
    int csock = cdata->csock;

    PlayerInfo *server = create_player();
    PlayerInfo *client = create_player();

    BattleMessage msg;
    srand(time(NULL));

    msg.type = MSG_INIT;
    strcpy(msg.message, "Seja bem vindo à Batalha Espacial!");
    send_message(csock, &msg);

    while(1) {
        msg.type = MSG_ACTION_REQ;
        send_message(csock, &msg);

        recv_message(csock, &msg);
        if (msg.type != MSG_ACTION_RES) break;

        int client_option = msg.client_action;
        int server_option = rand() % 5;

        client->opcao = client_option;
        server->opcao = server_option;

        int resultado = process_turno(client, server);

        msg.type = MSG_BATTLE_RESULT;
        msg.client_action = client->opcao;
        msg.server_action = server->opcao;
        msg.client_hp = client->hp;
        msg.server_hp = server->hp;
        msg.client_torpedoes = client->torpedos_usados;
        msg.client_shields = client->escudos_usados;
        send_message(csock, &msg);

        if (resultado != 2) {
            if (resultado == 1 || resultado == -1){
                msg.type = MSG_GAME_OVER;
                send_message(csock, &msg);
                break;
            }
            else if (resultado == 0) {
                msg.type = MSG_ESCAPE;
                send_message(csock, &msg);
                break;
            }
        }
    }

    free(server);
    free(client);
    close(csock);
    free(cdata);

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int server = socket(storage.ss_family, SOCK_STREAM, 0);
    if (server == -1) {
        logexit("socket");
    }

    int enable = 1;
    if(0 != setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(server, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(server, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("Bound to %s, waiting connections\n", addrstr);

    while(1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(server, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        struct client_data *cdata = malloc(sizeof(*cdata));
        if (!cdata) {
            logexit("malloc");
        }
        cdata->csock = csock;
        memcpy(&(cdata->storage), &storage, sizeof(storage));

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }

    exit(EXIT_SUCCESS);
}
