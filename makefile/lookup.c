/*
 * =====================================================================================
 *
 *       Filename:  dd.c
 *
 *    Description:  dd 
 *
 *        Version:  1.0
 *        Created:  2015年03月06日 17时52分50秒
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

//#define NELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

//int numarray[] = {123, 145, 512, 627, 800, 933};

int numeric (const int *p1, const int *p2){
    return(*p1 - *p2);
}

int lookup(int key, int *numarray, int len ){
    int *itemptr;
    // The cast of (int(*)(const void *,const void*)) is needed to avoid a type mismatch error at compile time
//    itemptr = (int *)bsearch (&key, numarray, NELEMS(numarray),
 //           sizeof(int), (int(*)(const void *,const void *))numeric);
    itemptr = (int *)bsearch (&key, numarray, len,
            sizeof(int), (int(*)(const void *,const void *))numeric);

    return (itemptr != NULL);
}

/*int main(void){
    if (lookup(512)){
        printf("512 is in the table.\n");
    }
    else{
        printf("512 isn't in the table.\n");
    }
    return 0;
}*/
