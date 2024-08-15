//
// Created by weslie on 2024/1/8.
//
#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define STR_MAX 2060
#define TABLE_NUM_MAX 1005
#define COMMAND_MAX 260

typedef struct list {
  struct list *head;
  struct list *tail;
} LinkedList;

LinkedList list[TABLE_NUM_MAX];

void Build(char str2, int list_id);
void Create();
void Init(int i);

int main() {

  for (int i = 0; i < TABLE_NUM_MAX; ++i) {
    Init(i);
  }

  while (1 == 1) {
    char str[STR_MAX];
    memset(str, 0, sizeof str);
    if (scanf("%s", str) != EOF) {
      if (strcmp(str, "CREATE") == 0) {
        Create()
      }

    } else {

    }

  }

  return 0;
}

void Init(int i) {
  list[i].head = NULL;
  list[i].tail = NULL;
}

void Build(char str2, int list_id) {
  int int_num[COMMAND_MAX], null_num[COMMAND_MAX], unique_num[COMMAND_MAX] ;
  char *char_num[COMMAND_MAX];
  int int_id = 0, char_id = 0, null_id = 0 , unique_id = 0 ;
  bool start_build = false, succeed = true ;
  int primary_num = 0 ;
  while (1 == 1) { //整个函数的大while
    char name[STR_MAX], type[20];
    scanf("%s %s", name, type);
    if (strcmp(name, "INT") == 0) {
      ++int_id;
    } else {
      ++char_id; //此处可能会有bug，当char出现语法错误时
      char len[STR_MAX];
      for (int i = 0, j = 0; i < strlen(type); ++i) {
        if (0 <= type[i] <= 9) {
          len[j] = type[i];
          ++j;
        }
      }
      int N = atoi(len);  //这一段找到N,有可能不需要
    }

    while (1 == 1) {//这个while对NOT NULL， PRIMARY KEY， UNIQUE 进行处理
      char string[STR_MAX];
      memset(string, 0, sizeof string);
      scanf("%s", string);
      if (strcmp(string, "NOT") == 0) { // 处理 NOT NULL情况
        char notnull[STR_MAX];
        memset(notnull, 0, sizeof notnull);
        scanf("%s", notnull);
        if (strcmp(notnull, "NULL") == 0) {
          null_num[null]
        } else {
          succeed = false;
          printf("SYNTAX ERROR\n");
          break;
        }
      } else if (strcmp(string, "PRIMARY") == 0) { //处理 PRIMARY KEY情况
        char string1[STR_MAX];
        memset(string1, 0, sizeof string1);
        scanf("%s", string1);
        if (strcmp(string1, "KEY") == 0) {
          ++primary_num;
        } else {
          succeed = false;
          printf("SYNTAX ERROR\n");
          break;
        }
      } else if (strcmp(string, "UNIQUE") == 0) { //处理unique问题  但下面过程中没对非unique的情况进行处理
        ++UniqueNum;
      } else if (strcmp(string, ",") == 0) { //遇到逗号，重新输入；
        break;
      } else if (strcmp(string, ");") == 0) {
        start_build = 1;
        break;
        //此处定义结束，开始构建结构体（没有对括号和分号间有换行符的情况处理）
      }

    }

    if(start_build){
      break ;
    }

  }

  if(succeed){
    struct
  }
}

void Create() {
  char str1[STR_MAX], table_name[STR_MAX], str3[STR_MAX];
  memset(str1, 0, sizeof str1);
  memset(str1, 0, sizeof table_name);
  memset(str1, 0, sizeof str3);
  scanf("%s %s %s", str1, table_name, str3);
  if (!(strcmp(str1, "TABLE") == 0 && strcmp(str3, "("))) {
    printf("SYNTAX ERROR\n");
    return;
  } else {
    Build

  }

}


