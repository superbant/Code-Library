/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017年10月20日 15时14分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "heap.h"


int main()  
{  
    int i = 0, heapSize = 10;  
    uint32_t array[] = {2,20,13,18,15,8,3,5,4,25};  
    uint32_t minelem = 10, maxelem = 1;  

    /*build min heap and test insert*/  
    MinHeapCreat(array, heapSize);    
    printf("Output the MinHeap:\n");    
    for(i = 0; i < heapSize; i++)    
    {    
        printf("%d\t", array[i]);    
    }    
    MinInsert(array, heapSize, minelem);   
    printf("\nOutput insert elem %d:\n",minelem);  
    for(i = 0; i < heapSize; i++)  
    {  
        printf("%d\t", array[i]);  
    }  
    printf("\n");  
    /*build max heap and test insert*/  
    MaxHeapCreat(array, heapSize);      
    printf("Output the MaxHeap:\n");      
    for(i = 0; i < heapSize; i++)      
    {      
        printf("%d\t", array[i]);      
    }      
    MaxInsert(array, heapSize,maxelem);   
    printf("\nOutput insert elem %d:\n",maxelem);      
    for(i = 0; i < heapSize; i++)      
    {      
        printf("%d\t", array[i]);      
    }  
    printf("\n");   
}  
