/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  test
 *
 *        Version:  1.0
 *        Created:  2015年06月29日 15时52分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "vp_log.h"
 
static  __attribute__((constructor)) void before()
{
 
    printf("Hello");
}

static  __attribute__((destructor)) void after()
{
    printf(" World!\n");
}



int main(int args,char ** argv)
{
#if 0
    log_open("test");
    log_error("mytest %s", "filename");
    log_warn("mytest %s", "filename");
    log_info("mytest %s", "filename");
    log_debug("mytest %s", "filename");
#else
    ZLOG_OPEN("mytest");
    ZLOG(ZLOG_STDOUT_INDEX, "zlog test %s\n", "zlog");
    ZLOGE("zlog test %s\n", "zlog");
    ZLOGW("zlog test %s\n", "zlog");
    ZLOGI("zlog test %s\n", "zlog");
    ZLOGD("zlog test %s\n", "zlog");
#endif
    return EXIT_SUCCESS;
}
