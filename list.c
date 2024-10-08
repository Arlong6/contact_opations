#include "ui.h"
#include "list.h"

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