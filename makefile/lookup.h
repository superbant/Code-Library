/*
 * =====================================================================================
 *
 *       Filename:  lookup.h
 *
 *    Description:  lookup.h
 *
 *        Version:  1.0
 *        Created:  2015年04月16日 09时52分06秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef DD_H
#define DD_H


#define NELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

int lookup(int key, int *numarray, int len );

#endif
