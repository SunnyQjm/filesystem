#include "list.h"

char temppath[100];
/**
 * 插入
 * @param list
 * @param code
 * @param fileName
 */
Node *insert(Node *list, char *code, char *fileName) {
    Node *node = malloc(sizeof(Node));
    strcpy(node->code, code);
    strcpy(node->data, fileName);
    node->next = NULL;

    if (list == NULL)    //空链表，直接将新建的链表作为链表头
        return node;

    Node *head = list;
    //找到最后的位置插入
    while (list->next != NULL) {
        list = list->next;
    }
    list->next = node;
    return head;
}

/**
 * 删除一个节点，成功后返回删除以后的头结点
 * @param list
 * @param code
 * @param deleteFileFunc
 * @return
 */
/**
 * 简单打印链表的信息
 * @param list
 */
void easyList(Node *list) {
    if(list == NULL)
        printf("list is NULL\n");
    while (list != NULL) {
        printf("%ld", strlen(list->code));
        printf("code: %s, data: %s\n", list->code, list->data);
        list = list->next;
    }
}

/**
 * 删除节点
 * @param list
 * @param code
 * @param deleteFileFunc        指向删除文件函数
 * @return
 */
Node *remove_(Node *list, char *code, int (*deleteFileFunc)(char *)) {
    Node *pre = NULL;
    Node *result = list;
    while (list != NULL) {
        if (strcmp(list->code, code) == 0) {   //找到提取码对应的文件
//            printf("delete->code: %s, data: %s\n", list->code, list->data);
            if (deleteFileFunc != NULL) {
                strcpy(temppath, DEFAULT_SAVE_PATH);
                strcat(temppath, list->code);
                strcat(temppath, "-");
                strcat(temppath, list->data);
                deleteFileFunc(temppath);
            }
            if (pre == NULL) {        //第一个节点就是目标节点
                result = list->next;
            } else {                //不是头结点，则直接跳过被删除节点，指向下一个节点即可
                pre->next = list->next;
            }
            //将被删除节点的空间释放
            free(list);
            break;
        }
        //记录当前节点，并把指针后移
        pre = list;
        list = list->next;
    }
    return result;
}

void freeAll(Node *list) {
    Node *pre = list;
    while (list != NULL) {
        list = list->next;
        free(pre);
        pre = list;
    }
}

int deleteFile(char *filePath) {
    return remove(filePath);
}

/**
 * 初始化，程序启动的时候把文件夹下的文件信息都加载进来
 * @param list
 * @param dirPath
 * @return
 */
Node *init(Node *list, char *dirPath) {
    DIR *dir = opendir(dirPath);
    char code[CODE_LENGTH];
    char name[MAX_DATA_LEN];
    struct dirent *entry;
    if (dir == NULL)
        return list;
    //遍历该目录下的所有文件
    while (entry = readdir(dir)) {
        //忽略掉当前目录和上机目录两个无效量
        if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
            continue;

        strncpy(code, entry->d_name, CODE_LENGTH);
        strcpy(name, entry->d_name + CODE_LENGTH + 1);
        list = insert(list, code, name);
    }
    return list;
}

Node* searchByCode(Node* list, char* code){
    while (list != NULL){
        if(strcmp(list->code, code) == 0){
            return list;
        }
        list = list->next;
    }
    return NULL;
}

//int main() {
//    Node *list;
//    list = init(list, "files");
//    easyList(list);
//    list = remove_(list, "1234", deleteFile);
//    easyList(list);
//    list = insert(list, "2564", "files/test.txt");
//    list = insert(list, "1234", "files/muko.jpg");
//    list = insert(list, "9748", "files/wow.jpg");
//    list = delete(list, "2564", deleteFile);
//    if (list == NULL) {
//        printf("list is NULL");
//    }
//    easyList(list);
//    freeAll(list);

//}