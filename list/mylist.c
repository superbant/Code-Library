#include "mylist.h"
#include <string.h>
#include <stdio.h>

typedef struct student
{
    char name[10];
    int age;
    int num;
    struct list_head list;
}Stu;

LIST_HEAD(g_stu);

int main()
{
    struct list_head * pos;
    Stu stu1;
    Stu stu2;
    Stu *tmpstu;
    strcpy(stu1.name ,"yanwf");
    stu1.age = 26;
    stu1.num =1;
    strcpy(stu2.name ,"linll");
    stu2.age = 23;
    stu2.num =3;

    list_add(&stu1.list, &g_stu);

    __list_for_each(pos, &g_stu)
    {
       Stu* pstu = list_entry(pos, Stu, list);
            printf("%s, %d, %d\n", pstu->name, pstu->age, pstu->num); 
    }

    list_for_each_entry(tmpstu, &g_stu, list)
    {
        printf("%s, %d, %d\n", tmpstu->name, tmpstu->age, tmpstu->num); 
    }

    list_replace(&stu1.list, &stu2.list);
    list_for_each_entry(tmpstu, &g_stu, list)
    {
        printf("%s, %d, %d\n", tmpstu->name, tmpstu->age, tmpstu->num); 
    }
    return 0;
}
