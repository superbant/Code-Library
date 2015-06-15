/*
 * =====================================================================================
 *
 *       Filename:  cc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年03月06日 18时01分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**输出数组元素
 *\param arr:指向数组的指针
 *\param len:数组元素的个数
 */

void print(char (*arr)[10], int len)
{
    int i;
    for (i=0;i<len;i++)
    {
        printf("%s ",arr[i]);
    }
    printf("\n");
}
/*
int main()
{
    char arr[][10]={"bac","bca","abc","acb","cba","cab"}; [> 定义二维字符数组<]
    char *key="bca";[> 要查找的字符串<]
    char *ptr=NULL; [> 字符指针<]
    [>输出未排序时字符数组的内容<]
    printf("before qsort :");
    print(arr,LENGTH(arr));
    [> 使用qsort对字符数组排序<]
    qsort((void *)arr,LENGTH(arr),sizeof(arr[0]),(int (*)(const void *,const void *))strcmp);
    [> 输出排序后字符数组的内容<]
    printf("after qsort :");
    print(arr,LENGTH(arr));
    [> 采用二分查找查找指定字符<]
    ptr=(char *)bsearch(key,arr,LENGTH(arr),sizeof(arr[0]),(int (*)(const void *,const void *))strcmp);
    if (ptr)
    { [> 找到<]
        printf("%s is in the array\n",key);
    }
    else[> 没找到<]
    {
        printf("%s isn't in the array\n",key);
    }
    return 0;
}*/
