//
// Created by ken on 2018/07/07.
//

#include "idobata.h"

/*--- 一つのノードを動的に生成 ---*/
static Node *alloc_node(void) {
    return calloc(1, sizeof(Node));
}

/*--- nの指すノードの各メンバに値を設定 ----*/
static void set_node(Node *n, const Member *x, const Node *next) {
    n->data = *x;        /* データ */
    n->next = next;        /* 後続ポインタ */
}

/*--- 線形リストを初期化 ---*/
void initialize(List *list) {
    list->head = NULL;        /* 先頭ノード */
    list->crnt = NULL;        /* 着目ノード */
}

/*--- 先頭にノードを挿入 ---*/
void insert_front(List *list, const Member *x) {
    Node *ptr = list->head;
    list->head = list->crnt = alloc_node();
    set_node(list->head, x, ptr);
}

/*--- 末尾にノードを挿入 ---*/
void insert_rear(List *list, const Member *x) {
    if (list->head == NULL)                    /* 空であれば */
        insert_front(list, x);                /* 先頭に挿入 */
    else {
        Node *ptr = list->head;
        while (ptr->next != NULL)
            ptr = ptr->next;
        ptr->next = list->crnt = alloc_node();
        set_node(ptr->next, x, NULL);
    }
}

/*--- 先頭ノードを削除 ---*/
void remove_front(List *list) {
    if (list->head != NULL) {
        Node *ptr = list->head->next;        /* ２番目のノードへのポインタ */
        free(list->head);                    /* 先頭ノードを解放 */
        list->head = list->crnt = ptr;        /* 新しい先頭ノード */
    }
}

/*--- 着目ノードを削除 ---*/
void remove_current(List *list) {
    if (list->head != NULL) {
        if (list->crnt == list->head)    /* 先頭ノードに着目していれば */
            remove_front(list);            /* 先頭ノードを削除 */
        else {
            Node *ptr = list->head;

            while (ptr->next != list->crnt)
                ptr = ptr->next;
            ptr->next = list->crnt->next;
            free(list->crnt);
            list->crnt = ptr;
        }
    }
}
