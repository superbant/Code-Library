/*
 * =====================================================================================
 *
 *       Filename:  bigheap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017年10月20日 14时48分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
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


/*大根堆调整*/  
void MaxHeapify(uint32_t* array, uint32_t heapSize, uint32_t currentNode)  
{  
    uint32_t leftChild = 0, rightChild = 0,  largest = 0;  
    leftChild = 2*currentNode + 1;  
    rightChild = 2*currentNode + 2;  
    if(leftChild < heapSize && array[leftChild] > array[currentNode])  
        largest = leftChild;  
    else  
        largest = currentNode;  
    if(rightChild < heapSize && array[rightChild] > array[largest])  
        largest = rightChild;  
    if(largest != currentNode)  
    {  
        Swap(array, largest, currentNode);  
        MaxHeapify(array, heapSize, largest);  
    }  
}  
  
/*构建大根堆*/  
void MaxHeapCreat(uint32_t* array, uint32_t heapSize)  
{  
    int i = 0;  
    for(i = heapSize/2; i >= 0; i--)  
    {  
        MaxHeapify(array, heapSize, i);  
    }  
} 


/*maintain the low N numbers*/  
void MaxInsert(uint32_t* array, uint32_t heapSize, uint32_t elem)  
{  
    if(elem < array[0])  
    {  
        array[0] = elem;  
        MaxHeapify(array, heapSize, 0);  
    }  
}  
