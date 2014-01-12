/*
 * util.c
 *
 *  Created on: 2009-5-26
 *      Author: lele
 */
#include "common.h"
/**
 * 解析URL 获得主机，端口，路径
 */
int parseURL(char* url,char* protocol,char* host,char* port,char* path)
{
	char *p,*q,*r;
	if(url==NULL){
		return 1;
	}
	p=url;
	while(*p&&(*p==' '||*p=='\t'||*p=='\r'||*p=='\n'))
		p++;
	/**
	 * 取得协议
	 */
	if ( protocol != NULL ) {
		r = p;
		q = protocol;
		while(*p&&*p!=':')
			*q++=*p++;
		*q='\0';
		if(*p!=':') {//找不到协议
			protocol[0] = '\0';
			p = r;
		}
		else {
		    p++;
		}
	}
	/**
	 * 取得主机和端口
	 *
	 */
	if ( host != NULL && port != NULL ) {
		if(*p=='/' && *(p+1)=='/') {
			p+=2;

			q = host;
			while(*p&&*p!='/')
			{
				*q++ = *p++;
			}
			*q='\0';
			q = host;
			while(*q&&*q!=':')
				q++;
			if(*q==':') {
				*q='\0';
				q++;
				strcpy(port,q);
			}
			else
			{
				port[0]='\0'; //未指明端口
			}
		}
		else//未指明主机和端口
		{
			host[0]='\0';
			port[0]='\0';
		}
	}
	/**
	 * 取得路径
	 *
	 */
	if ( path != NULL ) {
		q = path;
		while(*p)
			*q++ = *p++;
		*q = '\0';
	}
	return 0;
}

