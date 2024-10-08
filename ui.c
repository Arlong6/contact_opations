#include "ui.h"
#include "list.h"

void add_to_list(num_stor_head *list, num_storage *inform) {
    if (list->head == NULL) {
        list->head = inform;
        list->tail = inform;
    } else {
        list->tail->next = inform;
        inform->prev = list->tail;
        list->tail = inform;
    }
    list->count++;
}

void enter_information(num_storage *inform) {
    printf("Please enter name: \n");
    scanf("%49s", inform->name);  // 讀入 name 到結構體的 name 欄位，防止溢出

    char phone[10];
    while (1) {
        printf("Please enter phone (9 digits): \n");
        scanf("%9s", phone);  // 讀入 phone，限制長度

        // 清理緩衝區
        while (getchar() != '\n');
        
        if (is_valid_phone(phone)) {
            strcpy(inform->phone, phone); // 如果有效，將其複製到結構體
            break;
        } else {
            printf("Invalid phone number. Please retype again.\n");
        }
    }

    printf("Please enter email: \n");
    scanf("%49s", inform->mail);  // 讀入 email 到結構體的 mail 欄位，防止溢出
    // 清理緩衝區，防止多餘字符影響後續操作
    while (getchar() != '\n');
    inform->next = NULL;  // 初始化 next 和 prev
    inform->prev = NULL;
}

void free_(num_stor_head *list)
{
    // 釋放記憶體
    num_storage *curr = list->head;
    while (curr != NULL)
    {
        num_storage *temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(list);
}

// 函數用於檢查電話號碼的有效性
int is_valid_phone(const char *phone) {
    // 檢查電話號碼的長度和是否為數字
    if (strlen(phone) != 9) { // 假設電話號碼應為9位數
        return 0;
    }
    for (int i = 0; i < 9; i++) {
        if (!isdigit(phone[i])) {
            return 0; // 只允許數字
        }
    }
    return 1; // 有效
}