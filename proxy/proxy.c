#include "proxy.h"

char * inet_ultoa( u_int u, char * s)
{
    sprintf(s, "%d.%d.%d.%d",
            (unsigned int)(u>>24)&0xff, (unsigned int)(u>>16)&0xff,
            (unsigned int)(u>>8)&0xff, (unsigned int)u&0xff);
    return s;
}

u_int inet_atoul(const char * s)
{
    int i;
    int u[4];
    unsigned int rv;

    if(sscanf(s, "%d.%d.%d.%d", &u[0], &u[1], &u[2], &u[3]) == 4) 
    {
        for (i = 0, rv = 0; i < 4; i++) {
            rv <<= 8;
            rv |= u[i] & 0xff;
        }
        return rv;
    } 
    else
        return 0xffffffff;
}


int set_sock_opt(int sock, int level, int optname)
{
    int ret = 0;
    int reuse = 1;
    
    ret = setsockopt(sock, level, optname, &reuse, sizeof(reuse));
    if (ret < 0)
    {
        printf("setsockopt error %s\n", __FUNCTION__);
    }

    return ret;

}


int Bind(int sockfd, struct sockaddr_in saddr, int socklen)
{
    int ret = 0; 

    ret = bind(sockfd, (struct sockaddr *)&saddr, socklen);
    if (ret < 0)
    {
        printf("Bind() [%s:%d] error: %s",
                inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port), strerror(errno));
    }    
    return ret;
}

void init_sock_addr( struct sockaddr_in *sockaddr, u_int ip, u_short port)
{

    bzero(sockaddr, sizeof(struct sockaddr_in));
    sockaddr->sin_family = AF_INET;
    sockaddr->sin_addr.s_addr = htonl(ip);
    sockaddr->sin_port = htons(port);
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
    puts("Accept begin");
    int newfd = -1;

    if ((newfd = accept(s, addr, addrlen)) < 0) 
    {    
        if (errno != EAGAIN)
            printf("Accept() error: %s", strerror(errno));
    }    
    puts("Accept end");

    return newfd;
}


int Connect(int sockfd, struct sockaddr *serv_addr, socklen_t addrlen, int tm_out)
{
    puts("Connect begin");
    int ret = 0;
    unsigned long ul = 1;
    int error = -1;
    int len = sizeof(int);
    struct timeval tm;
    fd_set set;

    ioctl(sockfd, FIONBIO, &ul); //设置为非阻塞模式

    if ((ret = connect(sockfd, serv_addr, addrlen)) == -1)
    {
        tm.tv_sec  = tm_out;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(sockfd, &set);
        if (select(sockfd+1, NULL, &set, NULL, &tm) > 0)
        {
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
            if(error == 0) 
            {
                ret = 0;
            }
            else 
            {
                ret = -1;
            }
        } 
        else 
        {
            ret = -1;
        }
    }
    ul = 0;
    ioctl(sockfd, FIONBIO, &ul); //设置为阻塞模式

    if (ret < 0)
    {
        printf("connect [%s:%d] error: %s",
                inet_ntoa(((struct sockaddr_in*)serv_addr)->sin_addr),
                ntohs(((struct sockaddr_in*)serv_addr)->sin_port), strerror(errno));
        return -1;
    }
    puts("Connect end");
    return 0;
}


int Recv(int s, void * buf, int len, int flags)
{
    puts("Recv begin");
    int ret = -1;

    if ((ret = recv(s, buf, len, flags)) < 0) 
    {
        if (errno != ECONNRESET)
            printf("recv() error :%s", strerror(errno));
    }
    puts("Recv end");
    return ret;
}


int Send(int s, const char * buf, int len, int flags)
{
    puts("Send begin");
    int ret;
    int nsend = 0;
    int total = len;

    while (len > 0)
    {
        if ((ret = send(s, buf + nsend, total - nsend, flags)) < 0) 
        {
            if (errno != EPIPE)
                printf("send() error: [%d] %s", errno, strerror(errno));
            return -1;
        }
        nsend += ret;
        len -= nsend;
    }
    puts("Send end");
    return nsend;
}

int tcp_data_deal(int rsock, int ssock)
{
    puts("tcp_data_deal begin"); 
    char data_buf[BUFF_SIZE];
    int recvsize =0;
    int sendsize =0;
    int ret =0;
    if ((recvsize = Recv(rsock, data_buf, BUFF_SIZE, 0)) <= 0)
    {
        ret = -1; 
    }

    if (( sendsize = Send(ssock, data_buf, recvsize, MSG_NOSIGNAL)) <0)
    {
        ret =  -1;
    }
    puts("tcp_data_deal end"); 
    return ret;
}

int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
        return -1;
    return 0;
}


int insert_epoll_event(int epfd, int sockfd, int *curfds)
{
    struct epoll_event ev;

    ev.events = EPOLLIN /*| EPOLLET*/;
    ev.data.fd = sockfd;
    setnonblocking(sockfd);
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) 
    {
        printf("epoll add sockfd error: fd = %d", sockfd);
        return -1;
    }
    *curfds += 1;

    return 0;
}



void * run_tcp_proxy(void *arg)
{

    puts("run_tcp_proxy begin");
	int    ret = -1;
	int    cli_sock = -1;
	int    svr_sock = -1;
	struct sockaddr_in svr_addr;
    struct epoll_event events[MAXEPOLLSIZE];
    int epfd = 0, efd = 0, nfds = 0, i = 0;
    int curfds = 0;

	pproxy prun = (pproxy)arg;
	if (prun == NULL)
		goto __session_end;

	if ((svr_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		goto __session_end;

	init_sock_addr(&svr_addr, prun->dip, prun->dport);

	Connect(svr_sock, (struct sockaddr *)&svr_addr, sizeof(svr_addr), 5);

	prun->svr_sock = svr_sock;
	cli_sock = prun->cli_sock;
    
	if (set_sock_opt(svr_sock, SOL_SOCKET, SO_REUSEADDR) < 0
			|| set_sock_opt(cli_sock, SOL_SOCKET, SO_REUSEADDR) < 0)
		goto __session_end;
    printf("cli_sock: %d, svr_sock: %d\n", cli_sock, svr_sock);
    epfd = epoll_create(MAXEPOLLSIZE);
    if(epfd < 0)
    {
        fprintf(stderr, "epool_create error:%s\n", strerror(errno)); 
    }
    if (insert_epoll_event(epfd, cli_sock, &curfds) < 0)
        goto __session_end;

    if (insert_epoll_event(epfd, svr_sock, &curfds) < 0)
        goto __session_end;

	for(;;)
    {
        if ( curfds <= 0)
        {
            break; 
        }
	   
        nfds = epoll_wait(epfd, events, curfds, EPOLLWAITTIME);     

        if (nfds <= 0)
        {   
            continue ;
        }   
        
        for (i = 0; i < nfds; ++i)
        {
            efd = events[i].data.fd;

            if (efd == cli_sock )
            {
            
			    ret = tcp_data_deal(cli_sock, svr_sock);
               
            }

            if (efd == svr_sock )
            {
			    ret = tcp_data_deal(svr_sock, cli_sock);
            
            }

            if (ret < 0)
            {
                 epoll_ctl(epfd, EPOLL_CTL_DEL, svr_sock, NULL);
                 epoll_ctl(epfd, EPOLL_CTL_DEL, cli_sock, NULL);
                 curfds = 0;
                 break;
            }
        }
	}

    puts("run_tcp_proxy end");
__session_end:

	close(cli_sock);
	close(svr_sock);
    close(epfd);
	free(prun);
	return NULL;
}

void* start_tcp_proxy(void * args)
{
    puts("start_tcp_proxy begin");
    struct sockaddr_in cli_addr, svr_addr; 
    socklen_t cli_addr_len;
    int listenfd=-1, connfd = -1;
    char * ret = NULL;
    int tret = 0;
    pthread_t thread_id;

    pproxy pstart = (pproxy) args; 
    
    listenfd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0)
    {
        printf("socket error %s\n", __FUNCTION__); 
        ret = NULL;
    }

    set_sock_opt(listenfd, SOL_SOCKET, SO_REUSEADDR);
    
    init_sock_addr(&svr_addr, pstart->lip, pstart->lport);

    Bind(listenfd, svr_addr, sizeof(svr_addr));
    
    listen(listenfd, 20);
    
    cli_addr_len = sizeof(cli_addr);
    while(1)
    {
        connfd = Accept(listenfd, (struct sockaddr*)&cli_addr, &cli_addr_len); 

        pproxy prun = (pproxy)calloc(1, sizeof(proxy));
        if (prun == NULL)
        {
           printf("calloc error %s\n", __FUNCTION__); 
           continue;  
        }

        memcpy(prun, pstart, sizeof(proxy));

        prun->cli_sock = connfd;

        tret = pthread_create(&thread_id, NULL, run_tcp_proxy, (void *)prun);
        if (tret == 0)
        {
            pthread_detach(thread_id);
        }
    }
    puts("start_tcp_proxy end");
    free(pstart);
    return ret;
}

int load_tcp_proxy(int thread_state, void * args)
{
    int        tret = -1; 
    pthread_t  tid;


    puts("load_tcp_proxy begin");
    tret = pthread_create(&tid, NULL, start_tcp_proxy, (void *)args);
    puts("load_tcp_proxy end");
    if (tret != 0)
        return -1; 

    if (thread_state == T_WAITING)
        pthread_join(tid, NULL);
    if (thread_state == T_DETACH)
        pthread_detach(tid);

    return 1;
}

int load_tcp_proxy_args(int thread_state,u_long lip, u_short lport, u_long dip, u_short dport)
{

    pproxy args = (pproxy)calloc(1, sizeof(proxy));
    
    args->lip = lip;
    args->lport = lport;
    args->dip = dip;
    args->dport = dport;

    puts("__load_tcp_proxy_args begin");
    load_tcp_proxy(thread_state, (void*)args);
    puts("__load_tcp_proxy_args end");
    
    return 0;

}

int parseargs(int argc, char** argv)
{
    const char* const short_options = "l:p:s:o:";
    struct option long_options[] = {
        { "lip",         1,   NULL,    'l'},
        { "lport",       1,   NULL,    'p'},
        { "sip",        1,   NULL,    's'},
        { "sport",      1,   NULL,    'o'},
        {      0,            0,      0,     0}
    };
    char c;
    int ret = 0;

    while((c = getopt_long (argc, argv, short_options, long_options, NULL)) != -1)
    {    
        switch (c) {
            case 'l': 
                g_lip = inet_atoul(optarg);
                printf("localip=%s, g_lip:%x\n",optarg, g_lip);
                ret ++;
                break;
            case 'p': 
                g_lport = atoi(optarg);
                printf("localport=%s, g_lport:%hu\n", optarg, g_lport);
                ret ++;
                break;
            case 's': 
                g_dip = inet_atoul(optarg);
                printf("serverip=%s, g_dip:%x\n",optarg, g_dip);
                ret ++;
                break;
            case 'o': 
                g_dport = atoi(optarg);
                printf("serverport=%s, g_dport:%hu\n",optarg, g_dport);
                ret ++;
                break;
            case '?': 
                printf("illegal option: %c\n", c); 
                ret = 0;
                break;
            default: 
                printf("illegal option: %c\n", c); 
                ret = 0;
                break;
        }
    }    
    return ret;
}


int main(int argc, char **argv)
{
    if ( parseargs(argc, argv) < 4)
    {
        printf("Usage : %s \t--lip=<localip>\n"
                "\t\t\t--lport=<localport>\n"
                "\t\t\t--sip=<serverip>\n"
                "\t\t\t--sport=<serverport>\n", argv[0]);
        return 0;
    }
    load_tcp_proxy_args(T_WAITING,g_lip,g_lport,g_dip,g_dport);

    while(1);
    {
        sleep(1000); 
    }
    return 0;
}
