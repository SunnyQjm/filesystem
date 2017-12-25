#ifndef BASE_HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifndef TIME_H
#include <time.h>
#endif

#define RAND_LEN 4                  /*生成随机码的长度*/
#define RAND_START '0'              /*生成码的起始ASCII*/
#define RAND_END '9'                /*生成码的结束ASCII*/

/**
 * 调用本函数生成一个长度为RAND_LEN的随机码
 * 结果保存在randCode指向的区域
 * @param randCode
 */
void rand_code(char* randCode);