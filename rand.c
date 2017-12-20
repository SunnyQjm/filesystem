#include "rand.h"

void rand_code(char* randCode){
    char str[RAND_LEN + 1] = {0};
    int i;

    //通过当前时间设置随机种子，使得每次运行的结果具有一定的随机性
    srand(time(NULL));
    for(i = 0; i < RAND_LEN; i++){
        str[i] = rand() % (RAND_END - RAND_START + 1) + RAND_START;
    }
    strcpy(randCode, str);
}

//int main(){
//    char randCode[RAND_LEN + 1];
//    for(int i = 0; i < 10; i++){
//        rand_code(randCode);
//        printf("code: %s\n", randCode);
//    }
//}