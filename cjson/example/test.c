/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  cjson test 
 *
 *        Version:  1.0
 *        Created:  2017年09月07日 14时47分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"

typedef struct
{
    char firstName[32];
    char lastName[32];
    char email[64];
    int age;
    float height;
} PERSON;

struct record
{
    const char *precision;
    double lat;
    double lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};



void print_struct_person(PERSON person)
{
    printf("==========struct person begin===========\n");
    printf("firstName:%s\n", person.firstName); 
    printf("lastName:%s\n", person.lastName); 
    printf("email:%s\n", person.email); 
    printf("age:%d\n", person.age); 
    printf("height:%f\n", person.height); 
    printf("==========struct person end===========\n");
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    //简单key value
    char *json_string_keyvalue = "{\"firstName\":\"Brett\"}";
    printf("%s\n", json_string_keyvalue );
    cJSON *root = cJSON_Parse( json_string_keyvalue );    
    cJSON *item = cJSON_GetObjectItem(root, "firstName"); 
    printf("firstName:%s\n", item->valuestring);
    cJSON_Delete(root);

    //字符串到结构体
    char *json_string_struct="{\"person\":\
    {\
        \"firstName\":\"z\",\
            \"lastName\":\"jadena\",\
            \"email\":\"jadena@126.com\",\
            \"age\":8,\
            \"height\":1.17\
    }}";

printf("%s\n", json_string_struct);
root = cJSON_Parse(json_string_struct);

PERSON person;

memset(&person, 0x00, sizeof(person));

cJSON * object = cJSON_GetObjectItem(root, "person");
item = cJSON_GetObjectItem(object, "firstname");
memcpy(person.firstName,item->valuestring,strlen(item->valuestring));
item=cJSON_GetObjectItem(object,"lastName");
memcpy(person.lastName,item->valuestring,strlen(item->valuestring));
item=cJSON_GetObjectItem(object,"email");
memcpy(person.email,item->valuestring,strlen(item->valuestring));
item=cJSON_GetObjectItem(object,"age");
person.age=item->valueint;
item=cJSON_GetObjectItem(object,"height");
person.height=item->valuedouble;
cJSON_Delete(root);
print_struct_person(person);

//字符串结构体数组
char *json_string_struct_array="{\
                                \"people\":[\
                                {\"firstName\":\"z\",\"lastName\":\"Jason\",\"email\":\"bbbb@126.com\",\"height\":1.67},\
{\"lastName\":\"jadena\",\"email\":\"jadena@126.com\",\"age\":8,\"height\":1.17},\
{\"email\":\"cccc@126.com\",\"firstName\":\"z\",\"lastName\":\"Juliet\",\"age\":36,\"height\":1.55}\
    ]\
    }";

    root = cJSON_Parse(json_string_struct_array);
    cJSON *arrayItem= cJSON_GetObjectItem(root, "people"); 
    int i = 0;
    if (arrayItem !=NULL )
    {
        int size = cJSON_GetArraySize(arrayItem);
        PERSON * array_person = (PERSON*)calloc(size, sizeof(PERSON));   
        for (i=0;i<size;i++)
        {
            object=cJSON_GetArrayItem(arrayItem,i);
            item = cJSON_GetObjectItem(object, "firstname");
            if (item!=NULL)
            {
                memcpy(array_person[i].firstName,item->valuestring,strlen(item->valuestring));
            }
            item=cJSON_GetObjectItem(object,"lastName");
            if (item!=NULL)
            {
                memcpy(array_person[i].lastName,item->valuestring,strlen(item->valuestring));
            }
            item=cJSON_GetObjectItem(object,"email");
            if (item!=NULL)
            {
                memcpy(array_person[i].email,item->valuestring,strlen(item->valuestring));
            }
            item=cJSON_GetObjectItem(object,"age");
            if (item!=NULL)
            {
                array_person[i].age=item->valueint;
            }
            item=cJSON_GetObjectItem(object,"height");
            if (item!=NULL)
            {
                array_person[i].height=item->valuedouble;
            }
        }
        
        for(i=0;i<size;i++)
        {
            print_struct_person(array_person[i]);
        }
        free(array_person);
    }

    cJSON_Delete(root);

    //结构体数组到字符串
    struct record fields[2] =
    {
        {
            "zip",
            37.7668,
            -1.223959e+2,
            "",
            "SAN FRANCISCO",
            "CA",
            "94107",
            "US"
        },
        {
            "zip",
            37.371991,
            -1.22026e+2,
            "",
            "SUNNYVALE",
            "CA",
            "94085",
            "US"
        }
    };

    /* Our array of "records": */
    root = cJSON_CreateArray();
    cJSON * fld = NULL;
    for (i = 0; i < 2; i++)
    {
        cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", fields[i].address);
        cJSON_AddStringToObject(fld, "City", fields[i].city);
        cJSON_AddStringToObject(fld, "State", fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", fields[i].country);
    }

    char *out = cJSON_Print(root);
    printf("out:%s\n", out);
    cJSON_Delete(root);

    return EXIT_SUCCESS;


}
/* ----------  end of function main  ---------- */
