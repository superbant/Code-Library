/*
 * tcp.c
 *
 *  Created on: 2009-5-26
 *      Author: lele
 */

#include "common.h"
#include "error.h"

/**
 * 绑定端口
 */
int bind_port(int port)
{
	int netd;
	struct sockaddr_in addr;

	if((netd=socket(AF_INET, SOCK_STREAM, 0))<0) {
	        err_sys("不能建立套接字");
	  }
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if( bind(netd, (struct sockaddr*)&addr, sizeof(addr)) < 0 ) {
	  	err_sys("不能绑定到端口 ");
	     return -1;
	 }
	 if(listen(netd, 1)<0 ) {
		 err_sys("端口监听错误 ");
	       return -1;
	 }
	 return netd;
}
/**
 * 连接到主机
 */
int connect_to_server(char* server,int port)
{
	int sockfd;
	struct sockaddr_in serveraddr;
	struct hostent  *hp;
	 if((hp=gethostbyname(server)) == NULL )
	 {
		 printf("主机信息获取失败\n");
		 return -1;
	 }
	 if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0)
	 {
		 printf("套接字建立失败\n");
		 return -1;
	 }
	 bzero(&serveraddr,sizeof(serveraddr));
	 serveraddr.sin_family=AF_INET;
	 serveraddr.sin_port=htons(port);
	 char ip [32];
	 char **pptr;
	 pptr=hp->h_addr_list;
	 inet_ntop(hp->h_addrtype,*pptr,ip,sizeof(ip));
	 inet_pton(hp->h_addrtype,ip,&serveraddr.sin_addr);
	 int confd=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
//	 printf("server : %s \tport= %d \t socket id : %d \n",ip,port,sockfd);
	 if(confd<0)
	 {
		 printf("无法连接到主机\n");
		 return -1;
	 }

	 return sockfd;

}
