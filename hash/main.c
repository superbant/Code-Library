#include <stdio.h>
#include "addr_hash.h"
#include "hash.h"

#define HISTORY_LENGTH  20

typedef struct {
    long recv[HISTORY_LENGTH];
    long sent[HISTORY_LENGTH];
    long total_sent;
    long total_recv;
    int last_write;
} history_type;

hash_type *addr_hash;

history_type* history_create(void) {
    history_type* h;
    h = xcalloc(1, sizeof *h);
    return h;
}

int main(int argc,char **argv)
{
 addr_pair ap;

 history_type* ht;
 history_type* ht_find;

 union {
    history_type **ht_pp;
    void **void_pp;
 } u_ht = { &ht };

 union {
    history_type **ht_pp;
    void **void_pp;
 } u_ht_find = { &ht_find };

 ap.src.s_addr = 90;
 ap.src_port = 22;
 ap.dst.s_addr = 91;
 ap.dst_port = 22;
 ap.protocol = 17;
 long long aaa = 10909893;
 printf("aaa====%lf\n",aaa);

 addr_hash= addr_hash_create();

 if(hash_find(addr_hash,&ap,u_ht.void_pp) == HASH_STATUS_KEY_NOT_FOUND){
        printf("yes\n");
        ht = history_create();
 //ht->recv = 100101;
 //ht->sent= 22222;
 ht->total_sent = 33333;
 ht->total_recv= 44444;
 ht->last_write= 55555;
 printf("%ld\n",ht->total_sent);
 printf("%ld\n",ht->total_recv);
 printf("%ld\n",ht->last_write);
        hash_insert(addr_hash, &ap, ht);
    }
 if(hash_find(addr_hash,&ap,u_ht_find.void_pp) == HASH_STATUS_OK){
    printf("no\n");
    printf("ht_find->total_sent==%ld\n",ht_find->total_sent);
    printf("ht_find->total_recv==%ld\n",ht_find->total_recv);
    printf("ht_find->last_write==%d\n",ht_find->last_write);
 }
 printf("double long size is %d\n",sizeof(double long));

 return 0;
}
