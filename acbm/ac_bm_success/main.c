#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ac_bm.h"
pattern_tree *ptree_mail_subject;
int main(int argc,char** argv)
{
    unsigned int matched_indexs[32] = {0};
    int check_flag_title = 0;
    char buf_in[100];
    char *buf[3] = {"lipeiting","test","www"};
    pattern_data *patterns_subject = NULL;
    int i = 0;
    int subject_num = 3;
    patterns_subject = (pattern_data *) malloc (sizeof(pattern_data) * subject_num);

    if (!patterns_subject)
    {
        perror ("AC_BM pattern error ");
        exit (1);
    }

    for(i=0;i<subject_num;i++)
    {
        int str_len = 0;
        str_len = strlen(buf[i]);
        strcpy((patterns_subject+i)->data,buf[i]);
        (patterns_subject+i)->len = str_len;
    }
    ptree_mail_subject = acbm_init (patterns_subject, subject_num);

    scanf("%s",buf_in);
    //ptree_mail_subject = (pattern_tree *) malloc (sizeof(pattern_tree));	
    //if (!ptree_mail_subject)
    //{
    //	perror ("AC_BM pattern error ");
    //	exit (1);
    //}
    check_flag_title = acbm_search (ptree_mail_subject, buf_in, strlen (buf_in), matched_indexs, NMAX_LEN);
    // ======= debug =======		
    if (check_flag_title > 0)
    {
        int i = 0;
        for (i=0; i < check_flag_title; i++)
        {
            printf ("matched_indexs[%d] = %d\n", i, matched_indexs[i]);
        }
    }
    return 0;
}
