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

typedef struct node{
  int int_num , char_num;
  int index_int[COMMAND_MAX];
  char *index_char[COMMAND_MAX];
  int IorC; // int == 1, char == 0
  char form_name[STR_MAX] ;
  bool not_null;
  bool primary_key;
  bool unique;
  struct node *next;
}Node;


typedef struct list {
  char table_name[STR_MAX];
  Node *head;
  Node *tail;
} LinkedList;



LinkedList list[TABLE_NUM_MAX];

int IsEmpty(int i);
int Build(int i);
int Create(int list_num);
void insert(int i);
void Insert(int list_num);
void Init(int i);

int main() {

  for (int i = 0; i < TABLE_NUM_MAX; ++i) {
    Init(i);
  }

  int list_num = 0 ;
  while (1 == 1) {
    char str[STR_MAX];
    memset(str, 0, sizeof str);
    if (scanf("%s", str) != EOF) {
      if (strcmp(str, "CREATE") == 0) {//创建部分
        if(Create(list_num)){
          ++list_num;
        }
      } else if(strcmp(str, "INSERT") == 0){
        Insert()

      }

    } else {
      break;//输入完成 终止
    }

  }

  return 0;
}

void Init(int i) {
  list[i].head = NULL;
  list[i].tail = NULL;
}

int IsEmpty(int i){
  if(list[i].head == NULL && list[i].tail == NULL){
    return 1;
  }
  return 0 ;
}

int Build(int i){
  int primary_num = 0 , succeed = 0 ;
  while(!succeed) {//build 函数大循环
    Node *node = malloc(sizeof *node);
    node->not_null = false;
    node->primary_key = false;
    node->unique = false;  //初始化三个布尔变量

    char form_name[STR_MAX];
    memset(form_name, 0, sizeof form_name);
    scanf("%s", form_name);
    strcpy(node->form_name, form_name); //输入类型名
    char type[STR_MAX];
    scanf("%s", type);
    if (strcmp(type, "INT") == 0) {
      node->IorC = 1;
    } else {
      node->IorC = 0; //可能bug：1.char时语法出错，比如拼写不对等；2.后期插入时目标字符串大小超过N 这两个bug暂时不处理
    }

    while (1 == 1) { //此循环用于处理null primary unique
      // possible bug:
      // 1.more than one"NOT NULL" in line etc.
      // 2.SYNTAX ERROR when no "NULL" follows "NOT" etc.
      char str[STR_MAX];
      scanf("%s", str);
      if (strcmp(str, "NOT") == 0) { //NOT NULL处理
        char notnull[STR_MAX];
        scanf("%s", notnull);
        if (strcmp(notnull, "NULL") == 0) {
          node->not_null = true;
        }
      } else if (strcmp(str, "PRIMARY") == 0) {
        char key[STR_MAX]; //此处没有初始化
        if (strcmp(key, "KEY") == 0) {
          node->primary_key = true;
          ++primary_num;
        }
      } else if (strcmp(str, "UNIQUE") == 0) {
        node->unique = true;
      } else if (strcmp(str, ",") == 0) {
        break;
      } else if (strcmp(str, ");") == 0) {
        succeed = 1;
        break;
      }
    }
    if (IsEmpty(i)) {
      list[i].head = node;
    } else {
      list[i].tail->next = node;
    }
    list[i].tail = node;
    node->next = NULL;

  }

  if(primary_num == 1){
    printf("CREATE TABLE SUCCESSFULLY\n");
    return 1 ;
  } else {
    printf("ERROR\n");
    return 0 ;
  }


}


int Create(int list_num) {
  char str1[STR_MAX], table_name[STR_MAX], str3[STR_MAX];
  memset(str1, 0, sizeof str1);
  memset(str1, 0, sizeof table_name);
  memset(str1, 0, sizeof str3);
  scanf("%s %s %s", str1, table_name, str3);
  if (!(strcmp(str1, "TABLE") == 0 && strcmp(str3, "("))) {
    printf("SYNTAX ERROR\n"); //此处会重复打印ERROR，记得修改
    return 0;
  } else {
    strcpy(list[list_num].table_name, table_name); //未处理：如果表格已经存在则执行错误
    int judge = Build(list_num);
    if(judge){
      return 1;
    } else {
      return 0 ;
    }

  }

}
void insert(int i){
  while(1==1){
    Node *now = list[i].head;
    int num = 0 ;
    scanf("%d", &num);

    }
  }
}

void Insert(int list_num){
  char str1[STR_MAX], table_name[STR_MAX], str2[STR_MAX], str3[STR_MAX];
  scanf("%s %s %s %s", str1, table_name, str2, str3);
  if(!(strcmp(str1, "INTO") == 0 && strcmp(str2, "VALUES") == 0 && strcmp(str3, "(") == 0)){
    printf("SYNTAX ERROR\n") ;
    return ;
  } else {
    for(int i = 0 ; i < list_num; ++i){
      if(strcmp(list[i].table_name, table_name) == 0){
        insert(i);
      }
    }

  }
}


