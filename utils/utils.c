/*
 * =====================================================================================
 *
 *       Filename:  utils.c
 *
 *    Description:  工具库  
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
#include <unistd.h>
#include <errno.h>

// read "n" bytes from a descriptor 
ssize_t readn(int fd, void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nread;

    nleft = n;
    while(nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0 )
        {
            if (nleft == n) 
            {
                return -1;  //error, return -1;
            }
            else
            {
                break; // error, return amount read so far
            }
        }
        else if (nread == 0)
        {
            break; //EOF 
        }

        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft); //return >=0
}

// write "n" bytes to a descriptor
ssize_t writen(int fd, const void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;  

    nleft = n;
    while(nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) < 0 ) 
        {
            if (nleft == n) 
            {
                return -1; //error, return -1; 
            }
            else
            {
                break; // error, return amount written so far
            }
        }
        else if (nwritten == 0)
        {
            break; 
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n - nleft); // return >=0
}

//循环从文件描述符读取N字节
//-1:错误
//小于n：读到EOF
ssize_t readn_signal(int fd, void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0) 
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
            {
                nread = 0;
            }
            else
            {
                return -1; //错误，返回-1
                //break;//error, return amount read so far
            }
        }
        else if (nread == 0)
        {
            return -1; //错误，返回-1 对方套接字关闭 
            //break; //EOF, return amount written so far
        }

        nleft -= nread;
        ptr += nread;
    }
    return n - nleft; //return >=0
}

//循环往文件描述符fd写入N个字节, 返回值等于n表明写入成功，不等于n表明写入错误 
ssize_t writen_signal(int fd, const void *vptr, size_t n) 
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0)
    {
        if ( (nwritten = write(fd, ptr, nleft)) < 0)
        {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                nwritten = 0;
            }
            else
            {
                return -1; //错误，返回-1 
                //break; //error, return amount written so far
            }
        }
        else if (nwritten == 0)
        {
                return -1; //错误，返回-1 对方套接字关闭 
                //break; //error, return amount written so far
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n - nleft; // return >=0
}
