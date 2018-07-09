/*
 * idobata.h
 *
 *  Created on: 2018/07/02
 *      Author: b6122502
 */

#ifndef IDOBATA_H_
#define IDOBATA_H_

#include "mynet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 512
#define MSG_BUFSIZE 488

#define DEFAULT_PORT 50001 /* ポート番号既定値 */
#define SERVER_LEN 256     /* サーバ名格納用バッファサイズ */
#define USER_LEN 15        /* ユーザ名格納用バッファサイズ */

#define HELLO   1
#define HERE    2
#define JOIN    3
#define POST    4
#define MESSAGE 5
#define QUIT    6

/*-- パケット構造体 --*/
struct idobata {
    char header[4];   /* パケットのヘッダ部(4バイト) */
    char sep;         /* セパレータ(空白、またはゼロ) */
    char data[];      /* データ部分(メッセージ本体) */
};

/*-- ユーザ情報構造体 --*/
typedef struct {
    char username[USER_LEN];       /* ユーザ名 */
    int sock;                     /* ソケット番号 */
} Member;

/*--- ノード ---*/
typedef struct __node {
    Member data;            /* データ */
    struct __node *next;    /* 後続ポインタ（後続ノードへのポインタ）*/
} Node;

/*--- 線形リスト ---*/
typedef struct {
    Node *head;             /* 先頭ノードへのポインタ */
    Node *crnt;             /* 着目ノードへのポインタ */
} List;

/* client.c */
/*-- クライアント側の処理 --*/
void idobata_client(char *servername, int port_number, char *username);

/* server.c */
/*-- サーバ側の処理 --*/
void idobata_server(int port_number, char *username);

/* helo.c */
/* HELOパケットのブロードキャスト --*/
int helo(int port_number, char *servername);

/* util.c */
/*-- パケットの生成 --*/
void create_packet(int type, char *message, char *buf);

/*-- パケット解析 --*/
u_int32_t analyze_header(char *header);

/*-- 最大ディスクリプタを計算 --*/
int max_disc(int udp_sock, int tcp_sock, List *list);

/*-- 文字列の最後の改行コードを取り除く関数 --*/
char *chop_nl(char *s);

void print_member(List *list, char *server_username);

/* linkedlist.c */
/*--- 線形リストを初期化 ---*/
void initialize(List *list);

/*--- 先頭にノードを挿入 ---*/
void insertFront(List *list, const Member *x);

/*--- 末尾にノードを挿入 ---*/
void insert_rear(List *list, const Member *x);

/*--- 先頭ノードを削除 ---*/
void remove_front(List *list);

/*--- 着目ノードを削除 ---*/
void remove_current(List *list);


#endif /* IDOBATA_H_ */
