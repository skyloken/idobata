/*
 * idobata_server.c
 *
 *  Created on: 2018/07/02
 *      Author: b6122502
 */

#include "idobata.h"
#include <sys/select.h>
#include <signal.h>

/* サーバー側の処理 */
void idobata_server(int port_number, char *server_username) {
    printf("I am Server.\n");

    int udp_sock, tcp_sock;
    char s_buf[BUFSIZE];
    char r_buf[BUFSIZE];
    int strsize;
    char send_username[USER_LEN];
    int send_sock;

    /* UDP */
    struct sockaddr_in from_adrs;
    int from_len;

    /* TCP */
    int sock_accepted;

    /* パケット */
    struct idobata *packet;

    /* select */
    fd_set mask, readfds;
    int Max_sd;

    /* ユーザ情報 */
    List list;  // ユーザ情報を格納する線形リスト
    Node *ptr;

    /* UDPサーバの初期化 */
    udp_sock = init_udpserver((in_port_t) port_number);

    /* TCPサーバの初期化 */
    tcp_sock = init_tcpserver((in_port_t) port_number, 5);

    /* SIGPIPEを無視 */
    signal(SIGPIPE, SIG_IGN);

    /* ビットマスクの準備 */
    FD_ZERO(&mask);
    FD_SET(0, &mask);
    FD_SET(udp_sock, &mask);
    FD_SET(tcp_sock, &mask);

    /* ユーザ情報の初期化 */
    initialize(&list);

    /* 最大ディスクリプタの計算 */
    Max_sd = max_disc(udp_sock, tcp_sock, &list);

    /* メインループ */
    for (;;) {
        /*-- 受信データの有無をチェック --*/
        readfds = mask;
        select(Max_sd + 1, &readfds, NULL, NULL, NULL);

        /*-- UDPソケットの監視 --*/
        if (FD_ISSET(udp_sock, &readfds)) {
            from_len = sizeof(from_adrs);
            strsize = Recvfrom(udp_sock, r_buf, BUFSIZE, 0, (struct sockaddr *) &from_adrs, &from_len);
            r_buf[strsize] = '\0';
        }

        /*-- 新規TCPソケットの監視 --*/
        if (FD_ISSET(tcp_sock, &readfds)) {
            sock_accepted = Accept(tcp_sock, NULL, NULL);
            strsize = Recv(sock_accepted, r_buf, BUFSIZE, 0);
            r_buf[strsize] = '\0';
        }

        /*-- ユーザTCPソケットの監視 --*/
        if (list.head != NULL) {
            ptr = list.head;
            while (ptr != NULL) {
                if (FD_ISSET(ptr->data.sock, &readfds)) {
                    list.crnt = ptr;
                    send_sock = ptr->data.sock;
                    strcpy(send_username, ptr->data.username);
                    strsize = Recv(ptr->data.sock, r_buf, BUFSIZE, 0);
                    r_buf[strsize] = '\0';
                    break;
                }
                ptr = ptr->next;
            }
        }

        /*-- キーボード入力の監視 --*/
        if (FD_ISSET(0, &readfds)) {
            /* キーボードから文字列を取得し，POSTパケットを作成 */
            fgets(r_buf, MSG_BUFSIZE, stdin);
            send_sock = tcp_sock;
            strcpy(send_username, server_username);
            create_packet(POST, r_buf, r_buf);
        }

        /* ヘッダとデータに分割 */
        packet = (struct idobata *) r_buf;

        /*-- ヘッダに応じて分岐 --*/
        switch (analyze_header(packet->header)) {

            case HELLO:
                /*-- HELOパケットを受けとった時の処理 --*/

                /* HEREパケットをクライアントに送信する */
                create_packet(HERE, NULL, s_buf);
                Sendto(udp_sock, s_buf, strlen(s_buf), 0, (struct sockaddr *) &from_adrs, sizeof(from_adrs));

                break;

            case JOIN:
                /*-- JOINパケットを受けとった時の処理 --*/

                /* Usernameに改行があれば除く */
                chop_nl(packet->data);

                /* 新規メンバー登録処理 */
                Member x;
                x.sock = sock_accepted;
                strcpy(x.username, packet->data);
                insert_rear(&list, &x);
                FD_SET(sock_accepted, &mask);
                printf("%s Joined.\n", x.username);

                /* 全メンバー表示 */
                print_member(&list, server_username);

                /* 最大ディスクリプタ更新 */
                Max_sd = max_disc(udp_sock, tcp_sock, &list);

                break;

            case POST:
                /*-- POSTパケットを受けとった時の処理 --*/

                /* 発言ユーザ名を付加 */
                snprintf(s_buf, BUFSIZE, "[%s]%s", send_username, packet->data);
                if (send_sock != tcp_sock) {
                    /* 発言したクライアントからの発言を表示(サーバ自身は除く) */
                    printf("%s", s_buf);
                }

                /* MESGパケットの作成 */
                create_packet(MESSAGE, s_buf, s_buf);

                /* 他の全クライアントに対してメッセージを送信 */
                ptr = list.head;
                while (ptr != NULL) {
                    list.crnt = ptr;
                    if (send_sock != ptr->data.sock) {
                        /* 発言したクライアント以外に送信 */
                        Send(ptr->data.sock, s_buf, strlen(s_buf), 0);
                    }
                    ptr = ptr->next;
                }

                break;

            case QUIT:
                /*-- QUITパケットを受けとった時の処理 --*/
                printf("%s quit.\n", send_username);

                /* クライアントとの接続を閉じ、そのクライアントの接続情報を抹消 */
                FD_CLR(send_sock, &mask);
                close(send_sock);
                remove_current(&list);

                /* 全メンバー表示 */
                print_member(&list, server_username);

                /* 最大ディスクリプタ更新 */
                Max_sd = max_disc(udp_sock, tcp_sock, &list);

                break;

        }
    }
}