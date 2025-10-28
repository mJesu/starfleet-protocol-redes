#include "common.h"
#include "protocol.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Adicionar uma função process() para processar ataques e defesas
// Talvez a process() possa ser chamada pelo programa e a process() chama a print_msg(). Acho que faz sentido



typedef struct {
    int hp;
    int torpedos_usados;
    int escudos_usados;
    int opcao;
} PlayerInfo;

PlayerInfo* create_player() {
    PlayerInfo *player = malloc(sizeof(PlayerInfo));
    player->hp = 100;
    player->torpedos_usados = 0;
    player->escudos_usados = 0;
    player->opcao = -1;

    return player; // Criar jogador
}

const char* get_nome_acao(int acao) {
    switch (acao) {
        case 0: return "disparou um Laser!";
        case 1: return "disparou um Photon Torpedo!";
        case 2: return "ativou os Escudos.";
        case 3: return "usou Cloaking.";
        case 4: return "acionou o Hyper Jump!";
        default: return "ação desconhecida";
    }
}

void print_info_acoes() {
    printf("\nAções possíveis e seus códigos\n");
    printf("Código |      Ação\n");
    printf("   0   |  Laser Attack\n");
    printf("   1   | Photon Torpedo\n");
    printf("   2   |   Shields Up\n");
    printf("   3   |    Cloaking\n");
    printf("   4   |   Hyper Jump\n");
    printf("Informe sua escolha por meio do código> ");
}

void print_finalizacao(PlayerInfo *client, PlayerInfo *server, int vitoria, int turnos_jogados) {
    printf("Inventário Final\n");
    printf("- HP Restante: %d\n", client->hp);
    printf("- Turnos jogados: %d\n", turnos_jogados);
    printf("- Torpedos usados: %d\n", client->torpedos_usados);
    printf("- Escudos usados: %d\n", client->escudos_usados);
    if (vitoria == 1) {
        printf("Você derrotou a frota inimiga!\n");
    }
    else if (vitoria == -1) {
        printf("Sua nave foi destruída!\n");
    }
    else {
        if (server->opcao == 4) {
            printf("Servidor fugiu para o Hiper Espaço!\n");
        }
        else if (client->opcao == 4) {
            printf("Você fugiu para o Hiper Espaço!\n");
        }
    }
    printf("Obrigado por jogar!\n");
}

void print_mensagem_turno(int client_action, int server_action) {
    if (client_action == 0) { // Laser
        if (server_action == 0) { // Laser X Laser
            printf("Ambos receberam 20 de dano.");
        }
        else if (server_action == 1) { // Laser X Torpedo
            printf("Você recebeu 20 de dano.");
        }
        else if (server_action == 2) { // Laser X Escudos
            printf("Ataque bloqueado.");
        }
        else if (server_action == 3) { // Laser X Cloaking
            printf("Acerto! Nave inimiga perdeu 20 HP.");
        }
    } else if (client_action == 1) { // Torpedo
        if (server_action == 0) { // Torpedo X Laser
            printf("Acerto! Nave inimiga perdeu 20 HP.");
        }
        else if (server_action == 1) { // Torpedo X Torpedo
            printf("Ambos receberam 20 de dano.");
        }
        else if (server_action == 2) { // Torpedo X Escudos
            printf("Ataque bloqueado.");
        }
        else if (server_action == 3) { // Torpedo X Cloaking
            printf("Ataque falhou.");
        }
    } else if (client_action == 2) { // Escudos
        if (server_action == 0) { // Escudos X Laser
            printf("Ataque inimigo bloqueado.");
        }
        else if (server_action == 1) { // Escudos X Torpedo
            printf("Ataque inimigo bloqueado.");
        }
        else if (server_action == 2) { // Escudos X Escudos
            printf("Nada acontece.");
        }
        else if (server_action == 3) { // Escudos X Cloaking
            printf("Nada acontece.");
        }
    } else if (client_action == 3) { // Cloaking
        if (server_action == 0) { // Cloaking X Laser
            printf("Você recebeu 20 de dano.");
        }
        else if (server_action == 1) { // Cloaking X Torpedo
            printf("Ataque inimigo falhou.");
        }
        else if (server_action == 2) { // Cloaking X Escudos
            printf("Nada acontece.");
        }
        else if (server_action == 3) { // Cloaking X Cloaking
            printf("Nada acontece.");
        }
    }
}

void print_resultado_turno(PlayerInfo *client, PlayerInfo *server, int client_action, int server_action) {
    printf("Você %s\n", get_nome_acao(client_action));
    printf("Servidor %s\n", get_nome_acao(server_action));
    printf("Resultado: ");
    print_mensagem_turno(client_action, server_action);
    printf("\nPlacar: Você %d x Servidor %d\n\n", client->hp, server->hp);
}

void print_msg(MessageType msg) {
    switch (msg) {
        case MSG_INIT:
            printf("Seja bem-vindo ao StarFleet Protocol!\n");
            printf("Sua nave: SS-42 Voyager (HP: 100)\n");
            break;
        case MSG_ACTION_REQ:
            print_info_acoes();
            break;
        default:
            break;
    }
}

int process_turno(PlayerInfo *client, PlayerInfo *server) {

    if (client->opcao == 4) {
        return 0;
    }
    if (server->opcao == 4) {
        return 0;
    }

    if (client->opcao == 0) { // Laser
        if (server->opcao == 0) { // Laser X Laser
            client->hp -= 20;
            server->hp -= 20;
        }
        else if (server->opcao == 1) { // Laser X Torpedo
            client->hp -= 20;
        }
        else if (server->opcao == 2) { // Laser X Escudos
            // Nada acontece, pois o dano é bloqueado
        }
        else if (server->opcao == 3) { // Laser X Cloaking
            server->hp -= 20;
        }
    } else if (client->opcao == 1) { // Torpedo
        client->torpedos_usados++;
        if (server->opcao == 0) { // Torpedo X Laser
            server->hp -= 20;
        }
        else if (server->opcao == 1) { // Torpedo X Torpedo
            client->hp -= 20;
            server->hp -= 20;
        }
        else if (server->opcao == 2) { // Torpedo X Escudos
            // Nada acontece, pois o dano é bloqueado
        }
        else if (server->opcao == 3) { // Torpedo X Cloaking
            // Cloaking evita o dano
        }
    } else if (client->opcao == 2) { // Escudos
        client->escudos_usados++;
        if (server->opcao == 0) { // Escudos X Laser
            // Nada acontece, pois o dano é bloqueado
        }
        else if (server->opcao == 1) { // Escudos X Torpedo
            // Nada acontece, pois o dano é bloqueado
        }
        else if (server->opcao == 2) { // Escudos X Escudos
            // Nada acontece
        }
        else if (server->opcao == 3) { // Escudos X Cloaking
            // Nada acontece
        }
    } else if (client->opcao == 3) { // Cloaking
        if (server->opcao == 0) { // Cloaking X Laser
            client->hp -= 20;
        }
        else if (server->opcao == 1) { // Cloaking X Torpedo
            // Cloaking evita o dano
        }
        else if (server->opcao == 2) { // Cloaking X Escudos
            // Nada acontece
        }
        else if (server->opcao == 3) { // Cloaking X Cloaking
            // Nada acontece
        }
    }

    print_resultado_turno(client, server, client->opcao, server->opcao);

    if (client->hp <= 0) return -1; // Derrota
    if (server->hp <= 0) return 1; // Vitória

    return 2; // O jogo continua
}
