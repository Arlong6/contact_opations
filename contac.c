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

//ui
void add_to_list(num_stor_head *list, num_storage *inform);
void enter_information(num_storage *inform);
void free_(num_stor_head *list);
//db
int save_to_db(num_storage *inform);
int remove_contact(num_stor_head *list, const char *name);
void search(num_storage *inform, num_stor_head *list);
void add_information(num_storage *inform, num_stor_head *list);
void print_all_contacts();
//
int is_valid_phone(const char *phone);


int main(void)
{
    num_stor_head *list = (num_stor_head *)malloc(sizeof(num_stor_head));
    if (list == NULL) {
        printf("Allocation failed\n");
        return 0;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    while (1) {
        int input;
        num_storage *inform = (num_storage *)malloc(sizeof(num_storage)); // 動態分配記憶體

        if (inform == NULL) {
            printf("Allocation failed\n");
            break;
        }
        printf("please enter mode: 1 for add informations, -1 to get out, 2 to search, 3 to delete, 4 to check the rest datas: ");
        scanf("%d", &input);
        // 退出程式
        if (input == -1) break;
        //新增新的資料
        else if (input == 1) {

            add_information(inform, list);
        }
        else if (input ==2){
            search(inform, list);
        }
        else if (input == 3) {
            char name[100];
            printf("Please enter the name to remove: ");
            scanf("%s", name);
            remove_contact(list, name);
        }
        else if (input == 4) {
            print_all_contacts(); // 打印資料庫中的所有聯絡人
        }
    }

    free_(list);
    return 0;
}

void search(num_storage *inform, num_stor_head *list) {
    sqlite3 *db;
    sqlite3_stmt *res;
    char *err_msg = 0;

    // 打開資料庫
    int rc = sqlite3_open("contacts.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 準備 SQL 語句以搜尋特定名字
    const char *sql = "SELECT * FROM Contacts WHERE Name = ?";

    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // 綁定使用者輸入的名字
    printf("Please enter the name to search: ");
    scanf("%s", inform->name);
    sqlite3_bind_text(res, 1, inform->name, -1, SQLITE_STATIC);

    // 執行查詢並處理結果
    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW) {
        printf("Contact found:\n");
        printf("Name: %s\n", sqlite3_column_text(res, 0));
        printf("Phone: %s\n", sqlite3_column_text(res, 1));
        printf("Email: %s\n", sqlite3_column_text(res, 2));
    } else {
        printf("No contact found with the name %s.\n", inform->name);
    }

    // 清理並釋放資源
    sqlite3_finalize(res);
    sqlite3_close(db);
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

void add_information(num_storage *inform, num_stor_head *list)
{
    enter_information(inform); // 讀取使用者輸入
    add_to_list(list, inform); // 將輸入的資料加入鏈結串列

    // 將資料存入資料庫
    if (save_to_db(inform) == 0)
    {
        printf("Data saved to database successfully.\n");
    }
    else
    {
        printf("Failed to save data to database.\n");
    }

}

void enter_information(num_storage *inform) {
    printf("Please enter name: \n");
    scanf("%49s", inform->name);  // 讀入 name 到結構體的 name 欄位，防止溢出

    char phone[10];
    while (1) {
        printf("Please enter phone (9 digits): \n");
        scanf("%9s", phone);  // 讀入 phone，限制長度
        
        if (is_valid_phone(phone)) {
            strcpy(inform->phone, phone); // 如果有效，將其複製到結構體
            break;
        } else {
            printf("Invalid phone number. Please retype again.\n");
        }
    }

    printf("Please enter email: \n");
    scanf("%49s", inform->mail);  // 讀入 email 到結構體的 mail 欄位，防止溢出
    inform->next = NULL;  // 初始化 next 和 prev
    inform->prev = NULL;
}

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

void print_inform(num_stor_head *list) {
    num_storage *curr = list->head;
    while (curr) {
        printf("Name: %s\t", curr->name);
        printf("Phone: %s\t", curr->phone);
        printf("Email: %s\t", curr->mail);
        printf("\n");
        curr = curr->next;
    }
}

int save_to_db(num_storage *inform) {
    sqlite3 *db;
    sqlite3_stmt *res;
    char *err_msg = 0;

    // 打開資料庫
    int rc = sqlite3_open("contacts.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // 創建表（如果表不存在的話）
    const char *create_table_sql = "CREATE TABLE IF NOT EXISTS Contacts(Name TEXT, Phone TEXT, Email TEXT);";
    rc = sqlite3_exec(db, create_table_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    // 插入資料，使用參數化查詢
    const char *insert_sql = "INSERT INTO Contacts(Name, Phone, Email) VALUES(?, ?, ?);";
    rc = sqlite3_prepare_v2(db, insert_sql, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // 綁定變數到 SQL 語句
    sqlite3_bind_text(res, 1, inform->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(res, 2, inform->phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(res, 3, inform->mail, -1, SQLITE_STATIC);

    // 執行插入操作
    rc = sqlite3_step(res);
    if (rc != SQLITE_DONE) {
        printf("SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(res);
        sqlite3_close(db);
        return 1;
    }

    // 釋放資源
    sqlite3_finalize(res);
    sqlite3_close(db);
    return 0;
}

int remove_contact(num_stor_head *list, const char *name) {
    sqlite3 *db;
    char *err_msg = 0;

    // 打開資料庫
    int rc = sqlite3_open("contacts.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // 從資料庫中刪除指定聯絡人
    char delete_sql[256];
    snprintf(delete_sql, sizeof(delete_sql), "DELETE FROM Contacts WHERE Name='%s';", name);
    rc = sqlite3_exec(db, delete_sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    // 關閉資料庫
    sqlite3_close(db);

    // 從鏈結串列中刪除聯絡人
    num_storage *curr = list->head;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            // 找到聯絡人，從鏈結串列中移除
            if (curr->prev) {
                curr->prev->next = curr->next;
            } else {
                // 要刪除的是頭結點
                list->head = curr->next;
            }

            if (curr->next) {
                curr->next->prev = curr->prev;
            } else {
                // 要刪除的是尾結點
                list->tail = curr->prev;
            }

            free(curr);
            list->count--;
            printf("Contact '%s' deleted successfully.\n", name);
            return 0;
        }
        curr = curr->next;
    }

    printf("No contact found with the name '%s'.\n", name);
    return 0;
}


void print_all_contacts() {
    sqlite3 *db;
    sqlite3_stmt *res;
    const char *sql = "SELECT Name, Phone, Email FROM Contacts";

    // 打開資料庫
    int rc = sqlite3_open("contacts.db", &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 準備 SQL 查詢語句
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // 執行查詢並打印結果
    printf("All contacts in the database:\n");
    while (sqlite3_step(res) == SQLITE_ROW) {
        const unsigned char *name = sqlite3_column_text(res, 0);
        const unsigned char *phone = sqlite3_column_text(res, 1);
        const unsigned char *email = sqlite3_column_text(res, 2);
        printf("Name: %s\tPhone: %s\tEmail: %s\n", name, phone, email);
    }

    // 清理和關閉
    sqlite3_finalize(res);
    sqlite3_close(db);
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