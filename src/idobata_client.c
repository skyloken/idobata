/*
 * idobata_client.c
 *
 *  Created on: 2018/07/02
 *      Author: b6122502
 */
#include "idobata.h"

/* クライアント側の処理 */
void idobata_client(char *servername, int port_number, char *username) {
    printf("I am Client.\n");

    int sock;
    char s_buf[BUFSIZE], r_buf[BUFSIZE];
    int strsize;
    fd_set mask, readfds;
    struct idobata *packet;

    /* サーバに接続する */
    sock = init_tcpclient(servername, (in_port_t) port_number);
    printf("Connected.\n");

    /* JOINを送信 */
    create_packet(JOIN, username, s_buf);
    Send(sock, s_buf, strlen(s_buf), 0);

    /* ビットマスクの準備 */
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    FD_SET(sock, &mask);

    for (;;) {

        /* 受信データの有無をチェック */
        readfds = mask;
        select(sock + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &readfds)) {
            /* キーボードから文字列を入力する */
            fgets(s_buf, MSG_BUFSIZE, stdin);

            if (analyze_header(s_buf) == QUIT) {
                /* QUITパケットを送信 */
                create_packet(QUIT, s_buf, s_buf);
                Send(sock, s_buf, strlen(s_buf), 0);
                exit(EXIT_SUCCESS);
            } else {
                /* POSTパケットを送信 */
                create_packet(POST, s_buf, s_buf);
                Send(sock, s_buf, strlen(s_buf), 0);
            }
        }

        if (FD_ISSET(sock, &readfds)) {
            /* サーバからMESGパケットを受信する */
            strsize = Recv(sock, r_buf, BUFSIZE - 1, 0);
            r_buf[strsize] = '\0';
            packet = (struct idobata *) r_buf;
            if (analyze_header(packet->header) == MESSAGE) {
                printf("%s", packet->data);
                fflush(stdout);
            }
        }

    }

}