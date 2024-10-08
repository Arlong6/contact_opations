#include <ui.h>
#include <list.h>

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

