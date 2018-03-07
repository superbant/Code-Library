#include <stdio.h>
#include "multikeymatch.h"
/*daemo例子说明
 *按照关键字key匹配source字符串是否包含所要匹配的关键字
 * 匹配成功会打印出key值,并且打印key在字符串中的偏移位置
 *
 * */

int main()
{
	char source[] = "sdfwwwjs.baidu.baidudf123ssdfhaha0000哈哈fd$#$@$#@%哈哈";
	//struct match_stut key;
	char key1[]="ww";
	char key2[]=".baidu";
	char key3[]="test";

	list<struct result_struct> results;
	
	MultiKeyMatch mul;
	mul.AddKey(key1);
	mul.AddKey(key2);
	mul.AddKey(key3);
	//mul.MatchMain(source, &results, sizeof(source));
	mul.MatchInit();
	mul.MatchMain(source, &results, strlen(source));

	list<struct result_struct>::iterator it = results.begin();
	printf("test = %s\n", source);
	if(it == results.end())
		printf("not find\n");
	while(it != results.end())
	{
		printf("key = %s\n", (*it).key);
		printf("res = %d\n", (*it).res);
		it++;
	}
	return 0;
}
