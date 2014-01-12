/*
 * httpGet.c
 *
 *  Created on: 2009-5-26
 *      Author: lele
 */
#include "common.h"
#include "error.h"
int httpMethod(char* server,int port,char* path, char* param,int clientfd);
int setGETHeader(const char* path,const char* host,char* head);
int setPOSTHeader(const char* path,const char* host,const char* param,char*head);
int setHEADHeader(const char* path,const char* host,char* head);
static int method;
static char agentContent[]={"Mozilla/5.0 Gecko/2009042523 Ubuntu/9.04 (jaunty) Firefox/3.0.10"};
static char acceptContent[]={"text/html,application/xhtml+xml"};
/**
 * HTTP GET 方法
 */
int get(char * urls,char* opt,int clientfd)
{
	char servername[STRING_SIZE], path[STRING_SIZE];
	char portname[STRING_SIZE], protocol[STRING_SIZE];
	int port;
	parseURL(urls, protocol, servername, portname, path);
	port = atoi(portname);
	if ( protocol[0] == '\0'  &&  servername[0] == '\0'  &&  port == 0 ) {

	}
	if(strncmp("http",protocol,4)!=0) {
		err_sys("不能解析的协议");
	}

	if(port==0)
		port=80;
	method=0;
	httpMethod(servername,port,path,opt,clientfd);


	return 0;
}
/**
 * HTTP POST 方法
 */
int post(char * urls,char* param,int clientfd)
{
	char servername[STRING_SIZE], path[STRING_SIZE];
	char portname[STRING_SIZE], protocol[STRING_SIZE];
	int port;
	parseURL(urls, protocol, servername, portname, path);
	port = atoi(portname);
	if ( protocol[0] == '\0'  &&  servername[0] == '\0'  &&  port == 0 ) {

	}
	if(strncmp("http",protocol,4)!=0) {
		err_sys("不能解析的协议");
	}
	if(port==0)
		port=80;
	method=1;
	httpMethod(servername,port,path,param,clientfd);
	return 0;
}
/*
 * HTTP HEAD 方法
 */
int head(char * urls,int childfd)
{
	char servername[STRING_SIZE], path[STRING_SIZE];
	char portname[STRING_SIZE], protocol[STRING_SIZE];
	int port;
	parseURL(urls, protocol, servername, portname, path);
	port = atoi(portname);
	if ( protocol[0] == '\0'  &&  servername[0] == '\0'  &&  port == 0 )
	{
	}
	if(strncmp("http",protocol,4)!=0) {
		err_sys("不能解析的协议");
	}
	if(port==0)
	{
		port=80;
	}
	method=2;
	httpMethod(servername,port,path,0,childfd);
	return 0;
}

int httpMethod(char* server,int port,char* path, char* param,int clientfd)
{
	char buf [MAX_BUF_SIZE];
	char head [MAX_BUF_SIZE];
	int sockfd,writec,readc;
	int head_in,pos,http1;
	int server_status;
	http1=1;
	sockfd=connect_to_server(server,port);
	if(sockfd<0)
	{
		printf("与主机通信失败\n");
		//return 0;
	}
//	sprintf(buf,"GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nAccept: %s\r\n\r\n",path,server,agent,accept);
	switch(method)
	{
	case 0:
		setGETHeader(path,server,buf);
		break;
	case 1:
		//param--;
//		printf("POST param is %s \n",param);

		setPOSTHeader(path,server,param,buf);
		break;
	case 2:
		setHEADHeader(path,server,buf);
		break;
	default:
		break;

	}
	writec=write(sockfd,buf,sizeof(char)*strlen(buf));
//	printf("Write Byte: %d \n",writec);
//	printf("write Content %s \n",buf);

	shutdown(sockfd,SHUT_WR);

	memset(head,0,sizeof(char)*MAX_BUF_SIZE);

	head_in=1;
	/***
	 * 写HTTP头
	 */
	while(head_in&&(readc=read(sockfd,head,MAX_BUF_SIZE))>0)
	{
		pos=0;
//		printf("Return HEAD count=%d\t\n content =%s \n",readc,head);
		if(strncmp(head,"HTTP/1.",7)==0)
		{
			server_status=atoi(&head[9]);
			if(server_status==404)
			{
				printf("页面无法访问\n");
			}
			else if(server_status<200||server_status>400)
			{
				printf("忽视的请求\n");
				return 0;
			}
		}
		else
		{
			pos=readc;
			head_in=0;
			http1=0;
		}
		while(pos<=readc && !(head[pos]=='\n' && head[pos+1]=='\n') && !(head[pos]=='\n' && head[pos+1]=='\r' && head[pos+2]=='\n'))
		            pos++;
		if(pos<=readc) {
			 if(head[pos+1]=='\r')
				 pos+=3;
			 else
				 pos+=2;
			 //write(clientfd,head,pos);
			 while(pos<=readc && (head[pos]!='<')) //可能在<html> tag 前有一些乱七八糟的东西，去掉
				 pos++;
			 pos--;
			 write(clientfd,&head[pos],readc-pos); //写BODY

			 head[pos]='\0';
			 head_in=0;
		}
		else //得到的只是HEAD 的一部分
		{
			if(http1)
			{
				write(clientfd, head, readc);
				head[0]='\0';
			}
			else
			{
				write(clientfd,head,readc);
				head[0]='\0';
				head_in=0;
			}
		}
	}
	/**
	 * 继续读取流，得到的是HTTP 的内容
	 * 写HTTP 主体
	 */
	while((readc=read(sockfd,buf,MAX_BUF_SIZE))>0)
	{
		if(write(clientfd,buf,readc)!=readc)
		{
			printf("写回客户端出错\n");
		}
	}
	shutdown(sockfd,SHUT_RDWR);
	close(sockfd);

	return 0;
}
//设置 HTTP GET 头
int setGETHeader(const char* path,const char* host,char* head)
{
	sprintf(head,"GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent:"
			" %s\r\nAccept: %s\r\n\r\n",path,host,agentContent,acceptContent);
	return 0;
}
//设置HTTP POST 头
int setPOSTHeader(const char* path, const char* host,const char* param,char*head)
{
//	int length = strlen(param);
//	sprintf(head,"POST %s HTTP/1.1\r\nHost: %s\r\nUser-Agent:"
//				" %s\r\nAccept: %s\r\nContent-Type: %s\r\n"
//				"Connection: close\r\n"
//				"Content-Length: %d\r\n\r%s\r\n",
//				path,host,agentContent,acceptContent,contentType,length-2,param);
	sprintf(head,"POST %s HTTP/1.1\r\n%s",path,param);
	return 0;
}
//设置HTTP HEAD 头
int setHEADHeader(const char* path,const char* host,char* head)
{
	sprintf(head,"HEAD %s HTTP/1.1",path);
	return 0;
}
