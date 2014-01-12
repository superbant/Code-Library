/*
 * generate.c
 *
 *  Created on: 2009-5-29
 *      Author: lele
 */
#include "common.h"
#include <time.h>
static char status1[] ={"Method not allowed"};
static char status2[]={"Forbidden"};
static char status3[]={"Not Found"};
/**
 * 产生错误消息的头
 */
int generate_errorhead(int errorno,int childfd)
{
	char  buf [MAX_BUF_SIZE];
	time_t now;
	time(&now);
	char *status;
	switch(errorno)
	{
	case 403:
		status=status2;
		break;
	case 405:
		status=status1;
		break;
	default:
		status=status3;
	}
	sprintf(buf,"HTTP/1.1 %d %s\r\nServer: Apache-Coyote/1.1\r\nContent-Type: text/html;charset=utf-8\r\n"
			"Date:%s \r\n\r\n",
			errorno,status,ctime(&now));
	write(childfd,buf,strlen(buf));
	return 0;
}
/**
 * 产生405错误页面
 */
int generate_errorpage1(int childfd)
{
	char buf[MAX_BUF_SIZE];
	sprintf(buf,"<html><head><title>"
			"Method not allowed</title></head><body>"
			"<h1>405 Bad Request!</h1><h2>Method not allowed</h2>"
			"<p>Your request method has not been implemented,for more information,<br/>"
			"Please contact webadmin@nju.cn</p>\n");
	write(childfd,buf,strlen(buf));
	return 0;
}
/**
 * 产生403 错误页面
 */
int generate_errorpage2(int childfd)
{
	char buf[MAX_BUF_SIZE];
	sprintf(buf,"<html><head><title>"
			"Forbidden</title></head><body>"
			"<h1>403 Forbidden!</h1><h2>Forbidden!</h2>"
			"<p>Your request has been forbidden,for more information,<br/>"
			"Please contact webadmin@nju.cn</p>\n");
	write(childfd,buf,strlen(buf));
	shutdown(childfd,SHUT_RDWR);
	close(childfd);
	return 0;
}
