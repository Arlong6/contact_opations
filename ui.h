#ifndef __UI_H__
#define __UI_H__


#include <stdio.h>
#include <stdlib.h>

#include "list.h"

void add_to_list(num_stor_head *list, num_storage *inform);
void enter_information(num_storage *inform);
void free_(num_stor_head *list);
int is_valid_phone(const char *phone);



#endif