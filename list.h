#ifndef BASE_HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/*引入该头文件用于显示一个文件夹下面的文件信息*/
#include <dirent.h>

#define CODE_LENGTH 4                   /*提取码长度*/
#define MAX_DATA_LEN 30                 /*节点数据域的长度*/

#ifndef DEFAULT_SAVE_PATH
#ifndef DEFAULT_SAVE_PATH               /*默认保存上传文件的目录*/
#define DEFAULT_SAVE_PATH "upload/"
#endif
#endif

struct Node {
    char code[CODE_LENGTH];             /*提取码*/
    struct Node *next;                  /*指向下一个节点*/
    char data[MAX_DATA_LEN];            /*节点数据域*/
};

#define Node struct Node

/**
 * 插入
 * @param list
 * @param code
 * @param fileName
 */
Node *insert(Node *list, char *code, char *fileName);

/**
 * 简单打印链表的信息
 * @param list
 */
void easyList(Node *list);

/**
 * 删除一个节点，成功后返回删除以后的头结点
 * @param list
 * @param code
 * @param deleteFileFunc        指向删除文件函数
 * @return
 */
Node *remove_(Node *list, char *code, int (*deleteFileFunc)(char *));

/**
 * 释放链表
 * @param list
 */
void freeAll(Node *list);

/**
 * 删除文件函数
 * @param filePath
 * @return
 */
int deleteFile(char *filePath);

/**
 * 初始化，程序启动的时候把文件夹下的文件信息都加载进来
 * @param list
 * @param dirPath
 * @return
 */
Node *init(Node *list, char *dirPath);


/**
 * 通过提取码在链表中检索，如果存在，则返回目标节点，否则返回NULL
 * @param list
 * @param code
 * @return
 */
Node* searchByCode(Node* list, char* code);

/**
 * 通过文件名在文件链表中检索，如果存在，则返回目标节点，否则返回NULL
 * @param list
 * @param fileName
 * @return
 */
Node* searchByFileName(Node* list, char* fileName);