#ifndef UNP_H
#include "unp.h"
#endif
#define CODE_UPLOAD '0'
#define CODE_DOWNLOAD '1'
#define CODE_DISPLAY_FILE_LIST '2'
#define CODE_DELETE '3'
#define CODE_ERR '4'

#define FILE_SYSTEM_PORT 9748

#ifndef DEFAULT_SAVE_PATH
#define DEFAULT_SAVE_PATH "upload/"
#endif

#ifndef DEFAULT_DOWNLOAD_PATH
#define DEFAULT_DOWNLOAD_PATH "download/"
#endif


/**
 * 确保目录存在，不存在则创建
 * @param dirPath
 * @return -1 表示不存在并且创建失败， 0表示存在或不存在但创建成功
 */
int ensureDirectory(char* dirPath){
    int result = -2;  //用来保存创建文件夹的结果
    //判断保存下载文件的文件夹（download）存不存在，不存在则创建
    if(access(dirPath, F_OK) == -1){
        result = mkdir(dirPath, S_IRWXU);
    }

    if(result == 0){
        printf("创建文件夹\"%s\"成功!\n", dirPath);
    } else if(result == -1){
        err_quit("创建文件夹\"%s\"s失败!\n", dirPath);
    }

    return 0;
}

/**
 * 向指定的已连接的socket描述符，发送一个字符串
 */
void sendString(int sockfd, char *msg, char *buf) {
    strcpy(buf, msg);
    Writen(sockfd, buf, strlen(buf));
}

/**
 * 发送带协议码的消息
 * @param sockfd
 * @param msg
 * @param code
 * @param buf
 */
void sendWithCode(int sockfd, char *msg, char *code, char *buf) {
    strcpy(buf, msg);
    strcat(buf, code);
    //int size = strlen(msg) + strlen(code);
    Writen(sockfd, buf, strlen(buf));
}

/**
 * 获取文件的类型
 * @param filepath
 * @param savebuf
 * @return
 */
char *getFileType(char *filepath, char *savebuf) {
    int start = 0;
    int end = strlen(filepath);
    for (int i = end - 1; i >= 0; i--) {
        if (filepath[i] == '\\' || filepath[i] == '/') {
            start = i + 1;
            break;
        }
    }
    int j = 0;
    for (int i = start; i < end; i++, j++) {
        savebuf[j] = filepath[i];
    }
    savebuf[j] = '\0';
    return savebuf;
}
