/*
 * =====================================================================================
 *
 *       Filename:  utils.c
 *
 *    Description:  utils
 *
 *        Version:  1.0
 *        Created:  05/05/2016 07:53:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanwf 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
/* 
 * block socket
 * read "n" bytes from a descriptor  
 * the number of bytes read is returned  
 */
ssize_t readn_block(int fd, void *vptr, size_t n)
{
    size_t nleft = 0;
    ssize_t nread = 0;
    char *ptr = vptr;

    nleft = n;

    while(nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) <= 0 )
        {
            if (nread < 0)
            {
                perror("read"); 
            }
            break;
        }

        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft); 
}

/*
 * write "n" bytes to a descriptor 
 * the number of bytes writen is returned   
 */
ssize_t writen_block(int fd, void *vptr, size_t n)
{
    size_t nleft = 0;
    ssize_t nwritten = 0;  
    char *ptr = vptr;

    nleft = n;
    while(nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0 ) 
        {
            if (nwritten < 0 )
            {
                perror("write");           
            }
            break;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n - nleft);
}

/*
 * 循环从非阻塞文件描述符读取N字节
 * 判断返回值是否等于n
*/
ssize_t readn_nonblock(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0) 
    {
        if ((nread = read(fd, ptr, nleft)) <= 0)
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                nread = 0;
            }
            else
            {
                //nread = 0 : 对方套接字关闭
                //nread < 0 : 套接字错误
                if (nread < 0)
                {
                    perror("read"); 
                }
                else
                {
                    printf("the peer socket is closed\n"); 
                }
                break;
            }
        }

        nleft -= nread;
        ptr += nread;
    }
    return n - nleft; 
}

//循环往文件描述符fd写入N个字节, 返回值等于n表明写入成功，不等于n表明写入错误 
ssize_t writen_nonblock(int fd, const void *vptr, size_t n) 
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0)
    {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                nwritten = 0;
            }
            else
            {
                //nwritten= 0 : 对方套接字关闭
                //nwritten< 0 : 套接字错误
                if (nwritten < 0)
                {
                    perror("write"); 
                }
                else
                {
                    printf("the peer socket is closed\n"); 
                }
                break;
            }
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n - nleft; 
}
