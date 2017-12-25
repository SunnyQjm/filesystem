#ifndef UNP_H
#include "unp.h"
#endif
#include "fs.h"
#include <netinet/tcp.h>

#define DEFAULT_HOST_IP "127.0.0.1"
#define OPT_UPLOAD 0
#define OPT_DOWNLOAD 1
#define OPT_LIST_FILE 2
#define OPT_REMOVE 3
#define OPT_HELP 4
#define OPT_NUM 5

#define PROGRAM_NAME fs_cli


struct opt_info {
    char opt[5];
    char opt_description[15];
    char opt_usage[40];
};

struct opt_info opts[OPT_NUM] = {
        {"-u", "upload  ", "usage: ./fs_cli -u <file path>"},
        {"-d", "download", "usage: ./fs_cli -d <code>"},
        {"-l", "list    ", "usage: ./fs_cli -l"},
        {"-r", "remove  ", "usage: ./fs_cli -r <code>"},
        {"-h", "help    ", "usage: ./fs_cli <opt> [filepath/code]"}
};


void sendWithCode(int sockfd, char *msg, char *code, char *buf);

/**
 * 帮助接口，向用户展示程序的用法
 */
void help();

/**
 * 上传文件
 */
void upload(char *filepath, int sockfd);

/**
 * 凭码下载
 */
void download(char *code, int sockfd);

/**
 * 列出服务器的文件
 */
void list(int sockfd);

/**
 * 凭码删除
 */
void remove_f(char *code, int sockfd);

/**
 * 确保目录存在，不存在则创建
 * @param dirPath
 * @return -1 表示不存在并且创建失败， 0表示存在或不存在但创建成功
 */
int ensureDirectory(char* dirPath);

int main(int argc, char **argv) {
    if (argc < 2)
        err_quit("usage: ./fs_cli -h (to get the use instruction)");
    int opt = -1;

    //获取用户输入的操作类型
    for (int i = 0; i < OPT_NUM; i++) {
        if (strcmp(opts[i].opt, argv[1]) == 0) {
            opt = i;
        }
    }

    //如果输入的参数无效，则直接退出并提示调用help展示帮助
    if (opt == -1)
        err_quit("params error, usage: ./fs_cli -h (to get the use instruction)");

    //Socket 文件描述符
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(FILE_SYSTEM_PORT);
    Inet_pton(AF_INET, DEFAULT_HOST_IP, &servaddr.sin_addr);

    if (opt == OPT_HELP) {
        help();
        return 0;
    }
    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
    int flag = 1;
    printf("connect success: %d\n", sockfd);
    switch (opt) {
        case OPT_UPLOAD:
            if (argc != 3)
                err_quit("upload err, %s", opts[OPT_UPLOAD].opt_usage);
            upload(argv[2], sockfd);
            break;
        case OPT_DOWNLOAD:
            if (argc != 3)
                err_quit("download err, %s", opts[OPT_DOWNLOAD].opt_usage);
            download(argv[2], sockfd);
            break;
        case OPT_LIST_FILE:
            list(sockfd);
            break;
        case OPT_REMOVE:
            if (argc != 3)
                err_quit("remove err, %s", opts[OPT_REMOVE].opt_usage);
            remove_f(argv[2], sockfd);
            break;

    }
}


/**
 * 帮助接口，向用户展示程序的用法
 */
void help() {
    for (int i = 0; i < OPT_NUM; i++) {
        printf("%s:\t%s\n", opts[i].opt_description, opts[i].opt_usage);
    }
}



/**
 * 上传文件
 */
void upload(char *filepath, int sockfd) {
    int n = 0;
    printf("upload\n");
    printf("filepath: %s (%ld)\n", filepath, strlen(filepath));
    char sendbuf[MAXLINE];
    printf("filename: %s\n", getFileType(filepath, sendbuf));
    sendWithCode(sockfd, sendbuf, "0\n", sendbuf);

    FILE *fp = Fopen(filepath, "rb");
    while (!feof(fp)) {
        n = fread(sendbuf, sizeof(char), MAXLINE, fp);
        Writen(sockfd, (char *) sendbuf, n);
    }
    Fclose(fp);
    //关闭写一半，可让服务器端从Readline中解放出来
    Shutdown(sockfd, SHUT_WR);
    //文件传送完之后接收文件提取码
    while ((n = Readline(sockfd, sendbuf, MAXLINE)) > 0) {
        printf("%s\n", sendbuf);
    }
}

/**
 * 凭码下载
 */
void download(char *code, int sockfd) {
    int n = 0;
    char state_code;
    char sendbuf[MAXLINE];
    char filepath[100];
    sendWithCode(sockfd, code, "1\n", sendbuf);
    if ((n = Readline(sockfd, sendbuf, MAXLINE)) == 0) {
        err_quit("server error!!");
    } else {
        state_code = sendbuf[n - 2];
        sendbuf[n - 2] = '\0';
        //构造路径
        strcpy(filepath, DEFAULT_DOWNLOAD_PATH);
        strcat(filepath, sendbuf);

        //发送任意一行数据给服务器端回执
        sendString(sockfd, "nothing\n", sendbuf);
        //关闭写一端
        Shutdown(sockfd, SHUT_WR);

        //如果下载目录不存在，则先创建
        ensureDirectory(DEFAULT_DOWNLOAD_PATH);

        if(state_code == CODE_ERR){    //文件不存在，提取码无效
            printf("File not exists or code is not available. Please try again!");
        } else {
            FILE *fp = Fopen(filepath, "wb");
            if(fp == NULL) {
                return;
            }
            again:
            while ((n = Read(sockfd, sendbuf, MAXLINE)) > 0) {
                fwrite(sendbuf, sizeof(char), n, fp);
                printf("read: %d\n", n);
            }
            fflush(fp);
            if(n < 0 && errno == EINTR)
                goto again;
            Fclose(fp);

            printf("download success: %s\n", filepath);
        }
    }
}

/**
 * 列出服务器的文件
 */
void list(int sockfd) {
    int n = 0;
    char sendbuf[MAXLINE] = {0};
    sendWithCode(sockfd, sendbuf, "2\n", sendbuf);

    printf("-------------------------------File list-----------------------------\n\n");
    //接收文件列表
    while ((n = Readline(sockfd, sendbuf, MAXLINE)) > 0) {
        printf("%s\n", sendbuf);
    }
    printf("------------------------------    END   -----------------------------\n");
}

/**
 * 凭码删除
 */
void remove_f(char *code, int sockfd) {
    int n;
    char sendbuf[MAXLINE];
    sendWithCode(sockfd, code, "3\n", sendbuf);

    //关闭写一半
    Shutdown(sockfd, SHUT_WR);
    //接收删除的结果
    while ((n = Readline(sockfd, sendbuf, MAXLINE)) > 0) {
        printf("%s\n", sendbuf);
    }
}

