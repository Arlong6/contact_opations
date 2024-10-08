#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <string.h>
#include <ctype.h>

typedef struct num_storage {
    char name[100];
    char phone[10];
    char mail[30];
    struct num_storage *next;
    struct num_storage *prev;
} num_storage;

typedef struct num_stor_head {
    struct num_storage *head;
    struct num_storage *tail;
    int count;
} num_stor_head;

//db
int save_to_db(num_storage *inform);
int remove_contact(num_stor_head *list, const char *name);
void search(num_storage *inform, num_stor_head *list);
void add_information(num_storage *inform, num_stor_head *list);
void print_all_contacts();


#endif