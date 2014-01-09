#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h> 
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <getopt.h>

#define T_WAITING 1
#define T_DETACH  2 
#define BUFF_SIZE 1024
#define MAXEPOLLSIZE 1000
#define EPOLLWAITTIME 5000 //5秒


u_int g_lip;
u_short g_lport;
u_int g_dip;
u_short g_dport;

typedef struct proxy_args 
{
    u_int lip;
    u_int dip;
    u_short lport;
    u_short  dport;
    int cli_sock;
    int svr_sock;
}proxy, *pproxy;

