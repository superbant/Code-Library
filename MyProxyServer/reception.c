/*
 * reception.c
 *
 *  Created on: 2009-5-26
 *      Author: lele
 */

#include "common.h"
#include "error.h"
extern refuse_table_item table[];
extern int item_num;
int check(const char* host,const char* cmd,const char * clientname,int childfd);
int reception(int fd,struct sockaddr_in *from)
{
	char line [STRING_SIZE];
	char cmd [STRING_SIZE];
	char path [STRING_SIZE];
	char opt [STRING_SIZE];
	char requestline [MAX_BUF_SIZE];
	char host [STRING_SIZE];
	char* p,*q,*r,*re;
	int end_of_head;
	char * address= inet_ntoa(from->sin_addr);
	int add_close =0;

	ssize_t n;
	char buf [MAX_BUF_SIZE];
	 if((n = read(fd,buf,MAX_BUF_SIZE))<=0) {
	        err_sys("不能读取客户端内容");
	    }
	 buf[n]='\0';

	 /**
	  * 取得请求行
	  */
	 p = buf;
	 q = line;
	 while(*p&&*p!='\n'&&*p!='\r')
	    *q++ = *p++;
	 *q = '\0';
	 r = p;          /* store top of rest of command to 'r' */
	 if(*r=='\r') r++;
	 if(*r=='\n') r++; /* seek to next line */
	  p = line;

	  /**
	   * 取得请求方法 GET/POST/HEAD
	   */
	  q = cmd;
	  while(*p&&*p!=' '&&*p!='\t'&&*p!='\r'&&*p!='\n')
	     *q++ = *p++;
	   *q = '\0';
	   p++;
	   /*
	       * 取得请求路径
	       *
	    * */
	  q = path;
	  while(*p&&*p!=' '&&*p!='\t'&&*p!='\r'&&*p!='\n')
	      *q++ = *p++;
	  *q = '\0';

	  /*
	   * 取得HTTP版本
	   */
	  q=opt;
	  p++;
	  while(*p&&*p!=' '&&*p!='\t'&&*p!='\r'&&*p!='\n')
	  	     *q++ = *p++;
	  *q = '\0';
//	  printf("path = %s \t opt=%s \n",path,opt);

	  end_of_head=0;
	  p=r;//取下一行
	  re=requestline;
	  do
	  {
		  if(*p=='\r'||*p=='\n') {
//			  printf("end_of_line\n");
			  if(add_close==0) //添加连接请求行
			  {
				  strncpy(re,"Connection: close\r\n",19);
				  add_close=1;
				  re+=19;
			  }
			  if(strcmp("POST",cmd)==0)
			  {
				  while(*p=='r'||*p=='\n'||*p=='\0'||*p=='\t'||*p==' ')
				  {
					  *re++=*p++;
				  }
				  strcpy(re,p);
			  }
			  end_of_head = 1;
		     break;
		   }
		  if(strncmp("Host:",p,5)==0)
		  {
			  strncpy(re,"Host:",5);
			  re+=5;
			  p+=5;
			  while(*p==' ')
				  *re++=*p++;
			  q=host;
			  while(*p && *p!='\r' && *p!='\n')
				  *q++=*re++=*p++;
			  *q='\0';
		  }
		  if(strncmp("Connection:",p,11)==0)//更改连接请求行
		  {
			  strncpy(re,"Connection: close",17);
			  p+=11;
			  re+=17;
			  while(*p&&*p!='\r' && *p!='\n')
				  p++;

		  }
		  if(strncmp("Proxy-Connection:",p,17)==0) //将代理连接请求行去掉
		  {
			  p+=17;
			  while(*p&&*p!='\r' && *p!='\n')
				  p++;
			  while(*p=='\r'||*p=='\n')
				  p++;

		  }
		  while(*p&&*p!='\r'&&*p!='\n')
			  *re++=*p++;//跳过这行其余部分
		  if(*p=='\r')
			  *re++=*p++;
		  if(*p=='\n')
			  *re++=*p++; //读取下一行
		  if((!end_of_head) && (!*p)) {
			  if((n = read(fd,buf,MAX_BUF_SIZE))<=0) {
				  break;
			  }
			  else
			  {
				  buf[n]='\0';
				  p = buf;
				  break;
			  }
		  }
	  }while(!end_of_head);



	  if(strcmp("GET",cmd)==0)
	  {
		  if(check(host,cmd,address,fd))
		  {
			  printf("%s : 请求允许  %s %s \n",address,cmd,host);
			  get(path,requestline,fd);
		  }
	  }
	  else if(strcmp("POST",cmd)==0)
	  {
		  if(check(host,cmd,address,fd))
		  {
			  printf("%s : 请求允许  %s %s \n",address,cmd,host);
			  post(path,requestline,fd);
		  }
	  }
	  else if(strcmp("HEAD",cmd)==0)
	  {
		  if(check(host,cmd,address,fd))
		  {
			  printf("%s : 请求允许  %s %s \n",address,cmd,host);
			  head(path,fd);
		  }
	  }
	  else
	  {
		  printf("%s : 请求方法未知 %s %s \n",address,cmd,host);
		  generate_errorhead(405,fd);
		  generate_errorpage1(fd);
	  }
	 return 0;
}
int check(const char* host,const char* cmd,const char * clientname,int childfd)
{
	int i=0;
	for(;i<item_num;i++)
	{
		if(strcmp(host,table[i].addr)==0)
		{
			printf("%s : 地址过滤  %s %s \n",clientname,cmd,host);
			generate_errorhead(403,childfd);
			generate_errorpage2(childfd);
			return 0;
		}
	}
	return 1;
}
