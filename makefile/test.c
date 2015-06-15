/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  test
 *
 *        Version:  1.0
 *        Created:  2015年03月06日 16时38分35秒
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
#include <assert.h>
#include <stdlib.h>
#include <locale.h>
#include "lookup.h"
#include "print.h"

/* 
 * =========================================================================
 *         Name:  main
 *  Description:  main  
 * =========================================================================
 */
int main ( int argc, char *argv[] )
{
    /* Locale is set to "C" before this. This call sets it to the "current locale" by reading environment variables: */
    setlocale(LC_ALL, "");
    const struct lconv * const currentlocale = localeconv();
    printf("In the current locale, the default currency symbol is: %s\n", currentlocale->currency_symbol);
    div_t dt = div(5,2); 
    printf("quot:%d, rem:%d\n", dt.quot, dt.rem);
    fprintf(stdout, "quot:%d, rem:%d\n", dt.quot, dt.rem);

    int numarray[] = {123, 145, 512, 627, 800, 933};
    if (lookup(512, numarray, NELEMS(numarray))){
        printf("512 is in the table.\n");
    }
    else{
        printf("512 isn't in the table.\n");
    }

    char arr[][10]={"bac","bca","abc","acb","cba","cab"}; /* 定义二维字符数组*/
    char *key="bca";/* 要查找的字符串*/
    char *ptr=NULL; /* 字符指针*/
    /*输出未排序时字符数组的内容*/
    printf("before qsort :");
    print(arr,LENGTH(arr));
    /* 使用qsort对字符数组排序*/
    qsort((void *)arr,LENGTH(arr),sizeof(arr[0]),(int (*)(const void *,const void *))strcmp);
    /* 输出排序后字符数组的内容*/
    printf("after qsort :");
    print(arr,LENGTH(arr));
    /* 采用二分查找查找指定字符*/
    ptr=(char *)bsearch(key,arr,LENGTH(arr),sizeof(arr[0]),(int (*)(const void *,const void *))strcmp);
    if (ptr)
    { /* 找到*/
        printf("%s is in the array\n",key);
    }
    else/* 没找到*/
    {
        printf("%s isn't in the array\n",key);
    }

    return EXIT_SUCCESS;
}	
/* ----------  end of function main  ---------- */
