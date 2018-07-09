/*
 * idobata_util.c
 *
 *  Created on: 2018/07/02
 *      Author: b6122502
 */

#include "idobata.h"

/* パケットの生成 */
void create_packet(int type, char *message, char *buf) {
    char Buffer[BUFSIZE];

    switch (type) {
        case HELLO:
            snprintf(Buffer, BUFSIZE, "HELO");
            strcpy(buf, Buffer);
            break;
        case HERE:
            snprintf(Buffer, BUFSIZE, "HERE");
            strcpy(buf, Buffer);
            break;
        case JOIN:
            snprintf(Buffer, BUFSIZE, "JOIN %s", message);
            strcpy(buf, Buffer);
            break;
        case POST:
            snprintf(Buffer, BUFSIZE, "POST %s", message);
            strcpy(buf, Buffer);
            break;
        case MESSAGE:
            snprintf(Buffer, BUFSIZE, "MESG %s", message);
            strcpy(buf, Buffer);
            break;
        case QUIT:
            snprintf(Buffer, BUFSIZE, "QUIT");
            strcpy(buf, Buffer);
            break;
        default:
            /* Undefined packet type */
            break;
    }
}

/* パケット解析 */
u_int32_t analyze_header(char *header) {
    if (strncmp(header, "HELO", 4) == 0) return (HELLO);
    if (strncmp(header, "HERE", 4) == 0) return (HERE);
    if (strncmp(header, "JOIN", 4) == 0) return (JOIN);
    if (strncmp(header, "POST", 4) == 0) return (POST);
    if (strncmp(header, "MESG", 4) == 0) return (MESSAGE);
    if (strncmp(header, "QUIT", 4) == 0) return (QUIT);
    return 0;
}

/* 最大ディスクリプタを計算 */
int max_disc(int udp_sock, int tcp_sock, List *list) {

    int Max_sd = udp_sock;

    if (Max_sd < tcp_sock) {
        Max_sd = tcp_sock;
    }

    if (list->head != NULL) {
        Node *ptr = list->head;
        while (ptr != NULL) {
            if (Max_sd < ptr->data.sock) {
                Max_sd = ptr->data.sock;    //Max_sdに末尾の最大ディスクリプタを代入
            }
            ptr = ptr->next;
        }
    }

    return Max_sd;
}

/* 文字列の最後の改行コードを取り除く関数 */
char *chop_nl(char *s) {
    int len;
    len = strlen(s);
    if (s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
    return (s);
}

/* 全ユーザを表示 */
void print_member(List *list, char *server_username){
    Node *ptr;

    printf("-----------ALL USER-----------\n");
    printf("・%s(HOST)\n", server_username);
    ptr = list->head;
    while (ptr != NULL) {
        list->crnt = ptr;
        printf("・%s\n", ptr->data.username);
        ptr = ptr->next;
    }
    printf("------------------------------\n");
}
