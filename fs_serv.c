#include "unp.h"
#include "unpthread.h"

#define BASE_HEAD
#include "list.h"
#include "rand.h"

#define CODE_UPLOAD '0'
#define CODE_DOWNLOAD '1'
#define CODE_DISPLAY_FILE_LIST '2'
#define CODE_DELETE '3'
#define CODE_ERR '4'

#define MAX_SAVE_PATH_LEN 100
#ifndef DEFAULT_SAVE_PATH
#define DEFAULT_SAVE_PATH "files/"
#endif


void *deal(void *arg);

void sendWithCode(int sockfd, char *msg, char *code, char *buf);

Node* list = NULL;

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clilen;
    pthread_t tid;
    struct sockaddr_in cliaddr, servaddr;
    char buf[MAXLINE];

    //初始化链表
    list = init(list, DEFAULT_SAVE_PATH);
    easyList(list);
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(9748);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    for (;;) {
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
        Pthread_create(&tid, NULL, deal, &connfd);
    }
}


void sendWithCode(int sockfd, char *msg, char *code, char *buf) {
    strcpy(buf, msg);
    strcat(buf, code);
    //int size = strlen(msg) + strlen(code);
    Writen(sockfd, buf, strlen(buf));
}
/**
 * 向指定的已连接的socket描述符，发送一个字符串
 */
void sendString(int sockfd, char* msg, char* buf){
    strcpy(buf, msg);
    Writen(sockfd, buf, strlen(buf));
}

/**
 * 处理上传
 * @param connfd        已连接的socket描述符
 * @param recvbuf       接收缓存
 * @param extra         可能是文件名或者code
 */
void dealUpload(int connfd, char *recvbuf, char *extra) {
    int n = 0;
    char savePath[MAX_SAVE_PATH_LEN];
    char randCode[CODE_LENGTH];
    char fileName[MAX_SAVE_PATH_LEN];

    rand_code(randCode);
    strcpy(savePath, DEFAULT_SAVE_PATH);
    strcat(savePath, randCode);
    strcat(savePath, "-");
    strcat(savePath, extra);
    strcpy(fileName, extra);
    FILE *fp = Fopen(savePath, "wb");
    if(fp == NULL) {
        return;
    }
again:
    while ((n = Read(connfd, recvbuf, MAXLINE)) > 0) {
        fwrite(recvbuf, sizeof(char), n, fp);
    }
    if(n < 0 && errno == EINTR)
        goto again;

    fflush(fp);
    //文件接收完成，务必记得关闭文件描述符
    Fclose(fp);

    list = insert(list, randCode, fileName);
    if(list == NULL){
        printf("list == NULL\n");
    }
    printf("receive file success, save in: %s\n", savePath);
    //将提取码返回给客户端，客户端可以凭提取码下载文件
    sendString(connfd, "Upload success, code is: ", recvbuf);
    sendWithCode(connfd, randCode, "\n", recvbuf);
}

/**
 * 处理显示文件列表，将链表里的数据按一定格式返回即可
 * @param connfd
 * @param recvbuf
 */
void dealListFile(int connfd, char* recvbuf){
    Node* l = list;
    while(l != NULL){
        sendWithCode(connfd, l->data, "\n", recvbuf);
        l = l->next;
    }
}

/**
 * 处理删除操作
 * @param connfd
 * @param recvbuf
 */
void dealDelete(int connfd, char* recvbuf){
    list = remove_(list, recvbuf, deleteFile);
    sendString(connfd, "Operate success!\n", recvbuf);
}

/**
 * 处理下载
 * @param connfd
 * @param recvbuf
 */
void dealDownload(int connfd, char* recvbuf){
    int n = 0;
    Node* node = searchByCode(list, recvbuf);
    if(node == NULL){   //如果链表中没有检测到结果，则表示该提取码无效
        sendString(connfd, "The code is not available4\n", recvbuf);
    } else {
        char filepath[MAX_SAVE_PATH_LEN];
        strcpy(filepath, DEFAULT_SAVE_PATH);
        strcat(filepath, node->code);
        strcat(filepath, "-");
        strcat(filepath, node->data);

        printf("发送文件名\n");
        //存在则先把文件名发过去
        sendWithCode(connfd, node->data, "0\n", recvbuf);
        printf("文件名放成功\n");
        printf("路径：%s\n", filepath);

        //如果客户端接收到文件名，需要返回任意一行信息
        Readline(connfd, recvbuf, MAXLINE);
        //关闭读一端
        Shutdown(connfd, SHUT_RD);

        //将文件发给客户端
        FILE *fp = Fopen(filepath, "rb");
        while (!feof(fp)) {
            n = fread(recvbuf, sizeof(char), MAXLINE, fp);
            Writen(connfd, (char *) recvbuf, n);
            printf("write: %d\n", n);
        }
        Fclose(fp);
        printf("传输完毕\n");
    }
}

void *deal(void *arg) {
    //取出传入的socket套接字
    int connfd = *((int *) arg);
    int n;
    char state_code = 'a';
    char recvbuf[MAXLINE];

    //第一次先读取协议码，如果没有数据，则提示客户端并退出
    //有协议code，则按协议code处理
    if ((n = Readline(connfd, recvbuf, MAXLINE)) == 0) {
        sendWithCode(connfd, "Please input opt message", "4\n", recvbuf);
    } else {
        printf("n: %d", n);
        state_code = recvbuf[n - 2];
        recvbuf[n - 2] = '\0';
        printf("code: %c\n", state_code);
        printf("filename/code: %s\n", recvbuf);
        switch (state_code) {
            case CODE_UPLOAD:
                dealUpload(connfd, recvbuf, recvbuf);
                break;
            case CODE_DOWNLOAD:
                dealDownload(connfd, recvbuf);
                break;
            case CODE_DISPLAY_FILE_LIST:
                dealListFile(connfd, recvbuf);
                break;
            case CODE_DELETE:
                dealDelete(connfd, recvbuf);
                break;
            default:
                break;
        }
    }
    Close(connfd);
}

