/*
 * =====================================================================================
 *
 *       Filename:  smallheap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017年10月20日 14时33分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdint.h>
#include <stdlib.h>  
#include <stdint.h>  
#include <assert.h>  
#include <string.h>  
#include <stdio.h>  

static void Swap(uint32_t* array, uint32_t i, uint32_t j)  
{  
    assert(array);  
    uint32_t tmp = 0;  
    tmp = array[j];  
    array[j] = array[i];  
    array[i] = tmp;  
}  

/*小根堆调整*/  
void MinHeapify(uint32_t* array, uint32_t heapSize, uint32_t currentNode)  
{  
    uint32_t leftChild = 0, rightChild = 0,  minimum = 0;  
    leftChild = 2*currentNode + 1;  
    rightChild = 2*currentNode + 2;  
    if(leftChild < heapSize && array[leftChild] < array[currentNode])  
        minimum = leftChild;  
    else  
        minimum = currentNode;  
    if(rightChild < heapSize && array[rightChild] < array[minimum])  
        minimum = rightChild;  
    if(minimum != currentNode)  
    {  
        Swap(array, minimum, currentNode);  
        MinHeapify(array, heapSize, minimum);  
    }  
}  
/*构建小根堆*/  
void MinHeapCreat(uint32_t* array, uint32_t heapSize)  
{  
    int i = 0;  
    for(i = heapSize/2; i >= 0; i--)  
    {  
        MinHeapify(array, heapSize, i);  
    }  
}  

/*maintain the top N numbers*/  
void MinInsert(uint32_t* array, uint32_t heapSize, uint32_t elem)  
{  
    if(elem > array[0])  
    {  
        array[0] = elem;  
        MinHeapify(array, heapSize, 0);  
    }  
}  



