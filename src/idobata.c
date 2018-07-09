/*
 ============================================================================
 Name        : idobata.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "idobata.h"
#include <unistd.h>

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[]) {
    int port_number = DEFAULT_PORT;
    char servername[SERVER_LEN];
    char username[USER_LEN] = "Anonymous";
    int c;

    /* オプション文字列の取得 */
    opterr = 0;
    while (1) {
        c = getopt(argc, argv, "n:p:c:h");
        if (c == -1)
            break;

        switch (c) {
            case 'n': /* ユーザ名 */
                snprintf(username, 15, "%s", optarg);
                break;

            case 'p': /* ポート番号の指定 */
                port_number = atoi(optarg);
                break;

            case '?':
                fprintf(stderr, "Unknown option '%c'\n", optopt);
                break;

            case 'h':
                fprintf(stderr, "Usage: %s -n username -p port_number\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    /* サーバ or クライアント */
    if (helo(port_number, servername) == 0) {

        idobata_server(port_number, username);

    } else {

        idobata_client(servername, port_number, username);

    }

    // never reached
    exit(EXIT_SUCCESS);
}
