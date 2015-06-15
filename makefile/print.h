/*
 * =====================================================================================
 *
 *       Filename:  cc.h
 *
 *    Description:  cc.h
 *
 *        Version:  1.0
 *        Created:  2015年04月16日 09时53分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef CC_H
#define CC_H

#define LENGTH(x) sizeof(x)/sizeof(x[0])

void print(char (*arr)[10],int len);

#endif
