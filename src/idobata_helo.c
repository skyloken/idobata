/*
 * idobata_helo.c
 *
 *  Created on: 2018/07/02
 *      Author: b6122502
 */

#include "idobata.h"
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define TIMEOUT_SEC 2

/* HELOパケットのブロードキャスト */
int helo(int port_number, char *servername) {
    struct sockaddr_in broadcast_adrs;
    struct sockaddr_in from_adrs;
    socklen_t from_len;

    int sock;
    int broadcast_sw = 1;
    fd_set mask, readfds;
    struct timeval timeout;

    char s_buf[BUFSIZE], r_buf[BUFSIZE];
    int strsize;

    int i;
    int flag = 0;

    /* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
    set_sockaddr_in_broadcast(&broadcast_adrs, (in_port_t) port_number);

    /* ソケットをDGRAMモードで作成する */
    sock = init_udpclient();

    /* ソケットをブロードキャスト可能にする */
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcast_sw,
                   sizeof(broadcast_sw)) == -1) {
        exit_errmesg("setsockopt()");
    }

    /* ビットマスクの準備 */
    FD_ZERO(&mask);
    FD_SET(sock, &mask);

    /* HELOパケットの生成 */
    create_packet(HELLO, NULL, s_buf);
    strsize = strlen(s_buf);

    /* 4回HELOパケットを送る */
    for (i = 0; i < 4; i++) {

        /* HELOパケットをサーバに送信する */
        Sendto(sock, s_buf, strsize, 0, (struct sockaddr *) &broadcast_adrs,
               sizeof(broadcast_adrs));

        /* サーバからHEREを受信 */
        /* 受信データの有無をチェック */
        readfds = mask;
        timeout.tv_sec = TIMEOUT_SEC;
        timeout.tv_usec = 0;

        if (select(sock + 1, &readfds, NULL, NULL, &timeout) == 0) {
            printf("Time out.\n");
            continue;
        }

        from_len = sizeof(from_adrs);
        strsize = Recvfrom(sock, r_buf, BUFSIZE - 1, 0,
                           (struct sockaddr *) &from_adrs, &from_len);
        r_buf[strsize] = '\0';

        /* サーバーのアドレスを格納 */
        strcpy(servername, inet_ntoa(from_adrs.sin_addr));
        flag = 1;
        break;

    }

    close(sock); /* ソケットを閉じる */

    return flag;

}
