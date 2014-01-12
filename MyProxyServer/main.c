/*
 * main.c
 *
 *  Created on: 2009-5-26
 *      Author: lele
 */
#include "common.h"
#include "error.h"


/**
 * 并发服务器，处理僵死进程
 */
void ReleseChild();

refuse_table_item table[MAX_LIST_ITEM];//存储拒绝访问的网址
int item_num;//访问列表的条目数
static int con_count;

int main(int argc, char* argv[])
{
	int port = atoi(argv[1]);
	struct sockaddr_in from;
	pid_t childpid;
	int fd;
	socklen_t len;
	int netd;

	if(port==0)
	{
		err_sys("请输入合法的端口号\n");
	}
	int i=0;
	for(; i<argc-2;i++)
	{
		strcpy(table[i].addr,argv[i+2]);
	}
	item_num=argc-2;

	netd = bind_port(port);
	signal(SIGCHLD, ReleseChild);

	while(1) {
	        len = sizeof(from);
	        if((fd=accept(netd, (struct sockaddr*)&from, &len)) < 0 ) {
	            if(errno == EINTR)
	                continue;
	            exit(2);
	        }
	        con_count++;
	        if((childpid=fork())==0) {
	                    /* ----- Child process ----- */
	             close(netd);
	             reception(fd, &from);
	              exit(0);
	          }
	           else {
	              /* ----- Parent process ----- */
	               close(fd);
	          }
	}
	return 0;
}

void ReleseChild()
{
    /* union wait status; */

    while(wait3(NULL, WNOHANG, (struct rusage*) 0) >= 0 )
        ;
}


