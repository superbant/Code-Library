/*
 * =====================================================================================
 *
 *       Filename:  heap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017年10月20日 15时15分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdint.h>

 void MinHeapCreat(uint32_t* array, uint32_t heapSize);
 void MinHeapify(uint32_t* array, uint32_t heapSize, uint32_t currentNode);
 void MaxHeapCreat(uint32_t* array, uint32_t heapSize);
 void MaxHeapify(uint32_t* array, uint32_t heapSize, uint32_t currentNode);
 void Swap(uint32_t* array, uint32_t i, uint32_t j);
 void MaxInsert(uint32_t* array, uint32_t heapSize, uint32_t elem);
 void MinInsert(uint32_t* array, uint32_t heapSize, uint32_t elem);
