//
// Created by weslie on 2024/1/13.
//
/**
 * 还没考虑的问题：
 * 1.执行错误和语法错误先后
 * 4.许多关键字无法在字符串中出现
 * 7.排序过程，后续order by过程无法进行，即在无需继续排序时不知道后面有无语法错误
 */

#include<stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#define COMMANDS_WORD_BIG 2060
#define COMMANDS_NUM_BIG 260
#define BRACKETS_NUM_BIG 100
#define TABLE_NUM_BIG 105
#define STR_LEN_BIG 1005

char *key_word[24] = {"INT", "CHAR", "CREATE", "TABLE", "PRIMARY", "KEY", "NOT", "NULL", "UNIQUE", "INSERT", "INTO", "VALUES", "DELETE", "FROM", "WHERE", "UPDATE", "ORDER", "BY", "ASC", "DESC", "AND", "OR", "BETWEEN", "IS"};
char brackets[BRACKETS_NUM_BIG];
char *brackets_move = brackets;
typedef struct node {
  char name[STR_LEN_BIG];
  int IorC;
  int char_num;
  char *strings[COMMANDS_NUM_BIG];
  int strings_len;
  bool notnull, primary, unique;
  struct node *next;
} Node;

typedef struct list {
  char table_name[STR_LEN_BIG];
  Node *head;
  Node *tail;
} Linkedlist;

Linkedlist list[TABLE_NUM_BIG];

bool IsEmpty(int i);
void Init_brackets();
int BCS(char *str, int *jump, int *end);
int CheckName(char *str);  ///合法：return 1 不合法：return0 ；
int FindWord(char *string, char word);///查找某个字符，找到返回下标，否则返回-1
int FindWordBack(char *string, char word); ///从后往前查找
Node *GetSameName(int i, char *str);
void Init(int i);
void Init_node(int i);
void Init_node_insert(int this_list, int memory);
bool OperateStr(char *str1, char *operator, char *str2, int *sign);
bool OperateInt(char *str1, char *operator, char *str2, int *sign);
int Create(int i); ///-1 : SYNTAX ERROR  0 : ERROR  1 succeed
int Insert(int list_id, int *this_list, int *memory);
int Atom(int j, int result[], char *str);
int ConditionDecideType(char *value1, int *sign); ///找到字符串类型,顺便取掉字符串外引号
int Negetable(int j, int result[], char *condition_start);
int Conjunctable(int j, int result[], char *condition_start);
int Condition(int j, int result[], char *condition_start);
int Delete(int *count, int list_id, int delete_index[]);
int UpdateDecideType(char *value1, int *sign);
int Update(int list_id, int *count);
int ID_sort_primary(int this_list, int id_amount, int *ptr_id[]);
int ID_sort(int *ptr_id[COMMANDS_NUM_BIG], int id_amount, char *sort_name, int this_list);
int Select(int list_id);

int main() {
  int list_id = 0;
  for (int i = 0; i < TABLE_NUM_BIG; ++i) {
    Init(i);
  }
  int final = 0, se = 0; ///final : 是否EOF se ： 是否有分号
  while (!final) {
    char commands[COMMANDS_WORD_BIG];
    int commands_id = 0, quo = 0;//有无引号
    memset(commands, 0, sizeof commands);
    while (scanf("%c", &commands[commands_id]) != EOF) {//输入 start
      if (commands[commands_id] == '\n') {  ///去掉换行符
        commands[commands_id] = '\0';
      } else if(commands[commands_id] == '\'' && (commands[commands_id - 1] == ' ' || commands[commands_id - 1] == '(')){
        if(quo == 1){
          quo = 0 ;
        } else {
          quo = 1 ;
        }
        ++commands_id;
      }
      else{
        ++commands_id;
      }

      if (commands_id >= 1 && commands[commands_id - 1] == ';' && quo == 0) {  ///鉴定有无分号
        se = 1;
        break;
      }
    }//输入end commands_id 为该命令长度
    if (commands[0] == '\0') {
      return 0;
    }

    if (!se) {  ///若无分号 则EOF final改为1 手动添加分号 退出循环
      commands[commands_id] = ';';
      ++commands_id;
      final = 1;
    }

    char *word1 = strtok(commands, " ");
    if (strcmp(word1, "CREATE") == 0) {                       ///创建部分
      int judge = Create(list_id);
      if (judge == -1) {
        Init_node(list_id);
        Init(list_id);  /// 未free node 可能内存泄漏
        printf("SYNTAX ERROR\n");
      } else if (judge == 0) {
        Init_node(list_id);
        Init(list_id);
        printf("ERROR\n");
      } else {
        printf("CREATE TABLE SUCCESSFULLY\n");
        ++list_id;
      }
      Init_brackets();
    } else if (strcmp(word1, "INSERT") == 0) {           ///插入部分
      int this_list = -1, memory_char_num = -1;
      int judge = Insert(list_id, &this_list, &memory_char_num);
      if (judge == -1) {
        printf("SYNTAX ERROR\n");
        if (this_list != -1)Init_node_insert(this_list, memory_char_num);
      } else if (judge == 0) {
        printf("ERROR\n");
        if (this_list != -1) Init_node_insert(this_list, memory_char_num);
      } else {
        printf("1 RECORDS INSERTED\n");
      }

      Init_brackets();
    } else if (strcmp(word1, "DELETE") == 0) {
      int delete_array[COMMANDS_NUM_BIG] = {0};
      int delete_count = 0;
      int judge = Delete(&delete_count, list_id, delete_array);
      if (judge == -1) {
        printf("SYNTAX ERROR\n");
      } else if (judge == 0) {
        printf("ERROR\n");
      } else {
        printf("%d RECORDS DELETED\n", delete_count);
      }

    } else if (strcmp(word1, "UPDATE") == 0) {
      int count = 0;
      int judge = Update(list_id, &count);
      if (judge == -1) {
        printf("SYNTAX ERROR\n");
      } else if (judge == 0) {
        printf("ERROR\n");
      } else {
        printf("%d RECORDS UPDATED\n", count);
      }

    } else if (strcmp(word1, "SELECT") == 0) {
      int judge = Select(list_id);
      if (judge == -1) {
        printf("SYNTAX ERROR\n");
      } else if (judge == 0) {
        printf("ERROR\n");
      }
    } else if(strcmp(word1, ";") == 0){
      ;
    } else {
      printf("SYNTAX ERROR\n");
    }

  }

  return 0;
}

int FindWord(char *string, char word) { //查找某个字符，找到返回下标，否则返回-1
  for (int i = 0; i < strlen(string); ++i) {
    if (*(string + i) == word) {
      return i;
    }
  }
  return -1;
}

int FindWordBack(char *string, char word) {
  for (int i = strlen(string) - 1; i >= 0; --i) {
    if (*(string + i) == word) {
      return i;
    }
  }
  return -1;
}

Node *GetSameName(int i, char *str) {
  Node *now = list[i].head;
  while (now != NULL) {
    if (strcmp(now->name, str) == 0) {
      return now;
    }
    now = now->next;
  }
  return NULL;
}

void Init(int i) {
  memset(list[i].table_name, 0, sizeof list[i].table_name);
  list[i].head = NULL;
  list[i].tail = NULL;
  return;
}

void Init_node(int i) {
  Node *now = list[i].head;
  while (now != list[i].tail) {
    now = now->next;
    free(list[i].head);
    list[i].head = now;
  }
  free(list[i].tail);
  list[i].head = NULL;
  list[i].tail = NULL;
}
void Init_brackets() {
  while (brackets_move != brackets) {
    *(brackets_move - 1) = '\0';
    --brackets_move;
  }
}
int CheckName(char *str) {
  for (int j = 0; j < strlen(str); ++j) {
    if (!((str[j] >= 'A' && str[j] <= 'Z') ||
        str[j] >= 'a' && str[j] <= 'z' ||
        str[j] == '_'))
      return 0;
  }

  for(int i = 0 ; i < 24; ++i){
    if(strcmp(str, key_word[i]) == 0) return 0 ;
  }

  return 1;
}

void Init_node_insert(int this_list, int memory) {
  Node *now = list[this_list].head;
  while (now != NULL) {
    if (now->char_num != memory) {
      free(now->strings[memory]);
      now->char_num = memory;
      now->strings[memory] = NULL;
    } else {
      break;
    }
    now = now->next;
  }
  return;
}

bool OperateStr(char *str1, char *operator, char *str2, int *sign) {  ///
  if (strcmp(str1, "NULL") == 0 || strcmp(str2, "NULL") == 0) return false;
  if (strcmp(operator, "=") == 0) return (strcmp(str1, str2) == 0);
  if (strcmp(operator, "<") == 0) return (strcmp(str1, str2) < 0);
  if (strcmp(operator, ">") == 0) return (strcmp(str1, str2) > 0);
  if (strcmp(operator, "<>") == 0) return (strcmp(str1, str2) != 0);
  if (strcmp(operator, "<=") == 0) return (strcmp(str1, str2) <= 0);
  if (strcmp(operator, ">=") == 0) return (strcmp(str1, str2) >= 0);
  *sign = -1;
  return false;
}

bool OperateInt(char *str1, char *operator, char *str2, int *sign) {
  if (strcmp(str1, "NULL") == 0 || strcmp(str2, "NULL") == 0) return false;
  if (strcmp(operator, "=") == 0) return (atoi(str1) == atoi(str2));
  if (strcmp(operator, "<") == 0) return (atoi(str1) < atoi(str2));
  if (strcmp(operator, ">") == 0) return (atoi(str1) > atoi(str2));
  if (strcmp(operator, "<>") == 0) return (atoi(str1) != atoi(str2));
  if (strcmp(operator, "<=") == 0) return (atoi(str1) <= atoi(str2));
  if (strcmp(operator, ">=") == 0) return (atoi(str1) >= atoi(str2));
  *sign = -1;
  return false;
}

bool IsEmpty(int i) {
  if (list[i].head == NULL && list[i].tail == NULL) {
    return true;
  } else {
    return false;
  }
}

int BCS(char *str, int *ptr_jump, int *ptr_end) { ///修改掉字符串中的, ; )， 同时修改jump end值
  int num1 = FindWordBack(str, ',');     ///return 0 正常, return -1 语法错误（无左括号）
  int num2 = FindWordBack(str, ';');
  int num3 = FindWordBack(str, ')');
  if (num1 == strlen(str) - 1) {
    *(str + num1) = '\0';
    *ptr_jump = 1;
  }
  if (num2 == strlen(str) - 1) {
    *(str + num2) = '\0';
    *ptr_end = 1;
  }
  if (num3 == strlen(str) - 1) {
    *(str + num3) = '\0';
    if (brackets_move != brackets) {
      *(brackets_move - 1) = '\0';
      --brackets_move;
    } else {
      return -1;
    }
  }

  return 0;

}

int Create(int i) {///-1 : SYNTAX ERROR  0 : ERROR  1 succeed
  bool return0 = false;
  int primary_num = 0;
  bool left_brackets_or = false;  ///是否已找到（
  if (strcmp(strtok(NULL, " "), "TABLE") != 0) return -1;
  char *table_name = strtok(NULL, " ");
  strcpy(list[i].table_name, table_name);


  if (FindWord(list[i].table_name, '(') == strlen(list[i].table_name) - 1) {
    list[i].table_name[FindWord(list[i].table_name, '(')] = '\0';
    left_brackets_or = true;
    *brackets_move = '(';
    ++brackets_move;
  }

  if (!CheckName(list[i].table_name)) return -1;

  for (int j = 0; j < i; ++j) {
    if (strcmp(list[j].table_name, list[i].table_name) == 0) { ///表格存在，执行错误
      return0 = true;
    }

  }//table_name is OK
  if (!left_brackets_or) { //（没有跟在table_name后面情况, 找到并改为空格
    char *find_bra = table_name + strlen(table_name) + 1;
    while(1 == 1){
      if(*find_bra == ' '){
        ++find_bra;
      } else if(*find_bra == '('){
        *find_bra = ' ';
        *brackets_move = '(';
        ++brackets_move;
        left_brackets_or = true;
        break;
      } else {
        return -1 ;
      }
    }

  }

  int end = 0; //跳到结尾标志
  while (!end) {
    Node *node = malloc(sizeof *node);
    memset(node->name, 0, sizeof node->name);
    node->char_num = 0;
    node->notnull = false, node->primary = false, node->unique = false;
    for (int j = 0; j < COMMANDS_NUM_BIG; ++j) {
      node->strings[i] = NULL;
    }                                             ///初始化
    strcpy(node->name, strtok(NULL, " "));///name part
    if (!CheckName(node->name)) return -1;

    char *type = strtok(NULL, " "); ///type part
    int jump = 0;          ///jump 是否进行下面not null读入的标志;

    if (type == NULL) return -1;
    if (strcmp(type, "INT") == 0 || strcmp(type, "INT,") == 0
        || strcmp(type, "INT)") == 0 || strcmp(type, "INT);") == 0) {  ///判断int or char 同时找到char 长度
      node->IorC = 1;
    } else if (strncmp(type, "CHAR", 4) == 0) {
      node->IorC = 0;
      int temp_left_bracket = FindWord(type, '(');
      int temp_right_bracket = FindWord(type, ')');
      if (temp_left_bracket == -1 || temp_right_bracket == -1) return -1; ///未考虑：两括号间有空格
      char *num = malloc(100 * sizeof(char));
      char *num_move = num;
      for (int j = temp_left_bracket + 1; j < temp_right_bracket; ++j) {
        if (*(type + j) >= '0' && *(type + j) <= '9') {
          *num_move = *(type + j);
          ++num_move;
        } else {
          return -1;
        }
      }
      node->strings_len = atoi(num);
      free(num);
      *(type + temp_right_bracket) = '!';
    }
    if (BCS(type, &jump, &end) == -1) {
      return -1;
    }

    while (!jump && !end) {///用于处理not null primary key unique的循环
      char *possibility = strtok(NULL, " ");
      if (BCS(possibility, &jump, &end) == -1) {
        return -1;
      }

      if (strcmp(possibility, "NOT") == 0) { ///NOT NULL
        char *null = strtok(NULL, " ");
        if (BCS(null, &jump, &end) == -1) return -1;
        if (strcmp(null, "NULL") != 0) {
          return -1;
        } else {
          node->notnull = true;
        }
      } else if (strcmp(possibility, "PRIMARY") == 0) { ///PRIMARY KEY
        char *key = strtok(NULL, " ");
        if (BCS(key, &jump, &end) == -1) return -1;
        if (strcmp(key, "KEY") != 0) {
          return -1;
        } else {
          node->notnull = true;
          node->primary = true;
          node->unique = true;
          ++primary_num;
        }
      } else if (strcmp(possibility, "UNIQUE") == 0) {
        node->unique = true;
      } else {
        return -1;
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
  if (brackets_move != brackets) return -1;
  if (primary_num != 1 || return0) return 0; ///主键数量不为1， 执行错误；
  return 1;
}

int Insert(int list_id, int *this_list, int *memory) {
  bool return0 = false;
  if (strcmp(strtok(NULL, " "), "INTO") != 0) {///没有into直接-1
    return -1;
  }
  char *table_name = strtok(NULL, " ");
  if (!CheckName(table_name)) return -1;
  for (int i = 0; i < list_id; ++i) {
    if (strcmp(list[i].table_name, table_name) == 0) {
      *this_list = i;
    }
  }
  if (*this_list == -1) {
    return0 = true;///无法找到该表格,执行错误
    this_list = 0 ;
  }
  *memory = list[*this_list].head->char_num;
  char *values = strtok(NULL, " ");
  bool left_brackets_or = false;
  if(FindWord(values, '(') == strlen(values) - 1){
    *brackets_move = '(';
    ++brackets_move;
    *(values + FindWord(values, '(')) = '\0';
    left_brackets_or = true;
  }

  if(!left_brackets_or){
    char *find_bra = values + 7;
    while(1==1){
      if(*find_bra == ' '){
        ++find_bra;
      } else if(*find_bra == '('){
        *find_bra = ' ';
        *brackets_move = '(';
        ++brackets_move;
        left_brackets_or = true;
        break;
      } else {
        return -1;
      }
    }
  }
  if (strcmp(values, "VALUES") != 0) return -1;
  int end = 0; ///end终止标志（找到分号）;
  Node *now = list[*this_list].head;
  while (!end && now != NULL) {
    int jump = 0; ///jump 确定有无逗号
    now->strings[now->char_num] = malloc(STR_LEN_BIG * sizeof(char));/// 为要用的地方分配空间
    char *values_real = strtok(NULL, " ");
    char values_real1[STR_LEN_BIG];
    memset(values_real1, 0, sizeof values_real1);
    strcpy(values_real1, values_real);

    //if (BCS(values_real1, &jump, &end) == -1) return -1;
    int num1 = FindWord(values_real1, '\'');
    if (num1 == -1) { ///是数字和NULL的情况
      if(BCS(values_real1, &jump, &end) == -1) return -1 ;
      if(!jump && !end){
        char *hhh = strtok(NULL, " ");
        if(BCS(hhh, &jump, &end) == -1) return -1;
        if(strcmp(hhh, "") != 0 && !end) return -1;
      }

      if(strcmp(values_real1, "NULL") != 0){
        if (now->IorC != 1) {
          return0 = true;///类型错误 执行错误(如果是NULL就不管了）
        }

        for (int i = 0; i < strlen(values_real1); ++i) {
          if (!isdigit(*(values_real1 + i))) return -1;//数字部分出现字母
        }

      }

      strcpy(now->strings[now->char_num], values_real1);
      ++(now->char_num);
      now = now->next;

    } else if (num1 == 0) {  ///是字符串情况
      int num2 = -1;
      while(1==1){//循环作用：拼接字符串
        for(int i = 1 ; i < strlen(values_real1); ++i){
          if(*(values_real1 + i) == '\'' && *(values_real1 + i - 1) != '\\'){
            num2 = i;
            break;
          } ///找第二个引号
        }
        if (num2 != -1) {//找到第二个引号
          if (BCS(values_real1, &jump, &end) == -1) return -1;
          if(*(values_real1 + num2 + 1) != '\0') return -1; //防止'an'ma 这种引号外有字母出现
          if(jump || end) {
            break;
            ///找到引号和逗号(分号）过
          } else {///找到引号找不到逗号，去看后面是否是）；组合，是仍成功，不是报错
            char *hope = strtok(NULL, " ");
            if (BCS(hope, &jump, &end) == -1) return -1;
            if(strtok(NULL, " ") != NULL) return -1;
            if (end) break;
            return -1; ///找到引号找不到逗号，语法（其实不应该？）
          }
        } else {//能否找到引号(找不到第二个）
          *(values_real1 + strlen(values_real1)) = ' ';
          char *after = strtok(NULL, " ");
          strcat(values_real1, after);
        }
      }

      if (now->IorC != 0){
        return0 = true;
      }
      for (int i = num1 , j = 0; i <= num2; ++i, ++j) { ///赋值循环操作
        *(now->strings[now->char_num] + j) = *(values_real1 + i);
      }
      if (strlen(now->strings[now->char_num]) > (now->strings_len + 2)) return 0;/// 字符串超出长度范围 执行错误
      ++(now->char_num);
      now = now->next;
    } else {
      return -1;
    }
  }
  if (now != NULL || end != 1) { //输入与链表长度不匹配，执行错误
    return0 = true;
  }

  if (brackets_move != brackets) return -1;

  now = list[*this_list].head;
  while (now != NULL) {
    if (now->notnull && (strcmp(now->strings[now->char_num - 1], "NULL")) == 0) {
      return0 = true;                      ///检查本次输入中是否有notnull 但输入为NULL情况
    }
    if (now->unique) {
      for (int i = 0; i < now->char_num - 1; ++i) {
        if (strcmp(now->strings[i], now->strings[now->char_num - 1]) == 0) {
          return0 = true;                        ///检查本次输入是否与前面相等，满足unique
        }
      }
    }
    now = now->next;
  }

  if(return0) return 0 ;
  return 1;
}

int ConditionDecideType(char *value1, int *sign) { ///找到字符串类型
  int num1 = FindWord(value1, '\'');
  if (num1 == -1) { ///数字情况（无引号，疑似数字）
    for (int i = 0; i < strlen(value1); ++i) {
      if (!isdigit(*(value1 + i))) {
        *sign = 0;
        break;
      }
    }
    for (int i = 0; i < strlen(value1); ++i) {
      if (!isdigit(*(value1 + i)) && !isalpha(*(value1 + i)) && *(value1 + i) != '_') {
        *sign = -1;
        return -1;
      }
    }
    if (*sign == 1) {
      return 1;
    } else {
      return -1;
    }

  } else if (num1 == 0) {
    int num2 = -1;
    while (1 == 1) {
      for(int i = 1 ; i < strlen(value1); ++i){
        if(*(value1 + i) == '\'' && (*(value1 + i - 1) != '\\')){
          num2 = i;
          break;
        }
      }
///////////////////////////
      if (num2 != -1) {
        if(*(value1 + num2 + 1) != '\0') return -1;
        return 0;
      } else {
        char *after = strtok(NULL, " ");
        if (after == NULL) {
          *sign = -1;
          return -1;
        }
        *(value1 + strlen(value1)) = ' ';
        strcat(value1, after);
      }
    }

  } else {
    *sign = -1;
    return -1;
  }

}

char *AtomFieldOrConst(int this_list, Node *node, char *value, int for_num) {
  if (node != NULL) {
    return node->strings[for_num];
  } else {
    return value;
  }
}

int Atom(int j, int result[], char *str) {
  int atom_result[COMMANDS_NUM_BIG] = {0};
  int num1 = FindWord(str, '('), num2 = FindWordBack(str, ')');
  if ((num1 == -1 && num2 != -1) || (num1 != -1 && num2 == -1)) {
    return -1;
  } else if (num1 != -1 && num2 != -1) {  ///交给condition
    char *start = str, *end = str + strlen(str) - 1;
    while (1 == 1) {
      if (*start == ' ') {
        ++start;
      } else if (*start == '(') {
        break;
      } else {
        return -1;
      }
    }
    while (1 == 1) {
      if (*end == ' ') {
        --end;
      } else if (*start == ')') {
        break;
      } else {
        return -1;
      }
    }
    ++start, --end;
    char next[COMMANDS_WORD_BIG];
    memset(next, 0, sizeof next);
    strncpy(next, start, end - start + 1);
    int flag1 = Condition(j, atom_result, start);
    if (flag1 != 1)return flag1;

  } else { ///原子子句处理
    int IorC1 = -1, flag2 = 1;
    char *tem_value1 = strtok(str, " ");
    char value1[COMMANDS_WORD_BIG];
    memset(value1, 0, sizeof value1);
    strcpy(value1, tem_value1);
    Node *node1 = GetSameName(j, value1);
    if (node1 != NULL) {
      IorC1 = node1->IorC;
    } else {
      IorC1 = ConditionDecideType(value1, &flag2);
      if (flag2 != 1) return flag2;
    } ///完成第一个可比较值处理
    char *operator = strtok(NULL, " ");

    if (strcmp(operator, "IS") == 0) {
      int not = 0;
      char *no = strtok(NULL, " "), *null = NULL;
      if (strcmp(no, "NOT") == 0) {
        null = strtok(NULL, " ");
        not = 1;
      } else if (strcmp(no, "NULL") == 0) {
        null = no;
      } else {
        return -1;
      }
      if (strcmp(null, "NULL") != 0) return -1;
      char *tem = strtok(NULL, " ");
      if (tem != NULL) {
        return -1;
      } ///完成读入NULL


      if (node1 == NULL && not == 1) {
        for (int i = 0; i < list[j].head->char_num; ++i) {
          atom_result[i] = 1;
        }
      } else if (node1 == NULL && not == 0) { ;
      } else {
        for (int i = 0; i < list[j].head->char_num; ++i) {
          if ((strcmp(node1->strings[i], "NULL") == 0 && !not)
              || (strcmp(node1->strings[i], "NULL") != 0 && not)) {
            atom_result[i] = 1;
          }
        }
      }

    } else { ///非NULL情况
      char *tem_value2 = strtok(NULL, " ");
      char value2[COMMANDS_WORD_BIG];
      memset(value2, 0, sizeof value2);
      strcpy(value2, tem_value2);
      int IorC2 = -1, flag3 = 1;
      Node *node2 = GetSameName(j, value2);
      if (node2 != NULL) {
        IorC2 = node1->IorC;
      } else {
        IorC2 = ConditionDecideType(value2, &flag3);
        if (flag3 != 1) return flag3;
      } ///完成第二个可比较值处理

      if (IorC1 != IorC2) {
        return -1;
      }

      if (strcmp(operator, "BETWEEN") == 0) { ///判断是否between
        int nag = 1;
        char *and = strtok(NULL, " ");
        if (strcmp(and, "AND") != 0) {
          return -1;
        }

        char *tem_value3 = strtok(NULL, " ");
        char value3[COMMANDS_WORD_BIG];
        memset(value3, 0, sizeof value3);
        strcpy(value3, tem_value3);
        int IorC3 = -1;
        char *should_null = strtok(NULL, " ");
        if (should_null != NULL) {
          return -1;
        }
        Node *node3 = GetSameName(j, value3);
        if (node3 != NULL) {
          IorC3 = node3->IorC;
        } else {
          IorC3 = ConditionDecideType(value3, &nag);
          if (nag != 1) return nag;
        } ///完成第三个可比较值处理
        if (IorC3 != IorC1) {
          return -1;
        }

        if (IorC1) {
          for (int i = 0; i < list[j].head->char_num; ++i) {
            if (OperateInt(AtomFieldOrConst(j, node1, value1, i), ">=",
                           AtomFieldOrConst(j, node2, value2, i), &nag) &&
                OperateInt(AtomFieldOrConst(j, node1, value1, i), "<=",
                           AtomFieldOrConst(j, node3, value3, i), &nag)) {
              atom_result[i] = 1;
            }
          }
          if (nag != 1)return nag;
        } else {
          for (int i = 0; i < list[j].head->char_num; ++i) {
            if (OperateStr(AtomFieldOrConst(j, node1, value1, i), ">=",
                           AtomFieldOrConst(j, node2, value2, i), &nag) &&
                OperateStr(AtomFieldOrConst(j, node1, value1, i), "<=",
                           AtomFieldOrConst(j, node3, value3, i), &nag)) {
              atom_result[i] = 1;
            }
          }
          if (nag != 1)return nag;
        }

      } else {  ///非between情况
        int flag4 = 1;
        if (IorC1 == 0) { ///比较字符串
          for (int i = 0; i < list[j].head->char_num; ++i) {
            if (OperateStr(AtomFieldOrConst(j, node1, value1, i), operator,
                           AtomFieldOrConst(j, node2, value2, i), &flag4)) {
              atom_result[i] = 1;
            }
            if (flag4 != 1) return flag4;
          }
        } else {  ///比较INT型
          for (int i = 0; i < list[j].head->char_num; ++i) {
            if (OperateInt(AtomFieldOrConst(j, node1, value1, i), operator,
                           AtomFieldOrConst(j, node2, value2, i), &flag4)) {
              atom_result[i] = 1;
            }
          }
          if (flag4 != 1) return flag4;
        }

      }
    }
  }
  for (int i = 0; i < list[j].head->char_num; ++i) {
    result[i] = atom_result[i];
  }
  return 1;
}
int Negetable(int j, int result[], char *condition_start) {
  int not = 0;
  char *start_move = condition_start;
  while (*start_move == ' ') ++start_move;
  char *next_start = NULL;
  if (strncmp(start_move, "NOT", 3) == 0) {
    not = 1;
    next_start = start_move + 3;
  } else {
    next_start = start_move;
  }

  int negetable_result[COMMANDS_NUM_BIG] = {0};
  int flag = Atom(j, negetable_result, next_start);
  if (flag != 1)return flag;
  if (not) {
    for (int i = 0; i < list[j].head->char_num; ++i) {
      if (negetable_result[i] == 1) {
        result[i] = 0;
      } else if (negetable_result[i] == 0) {
        result[i] = 1;
      }
    }
  } else {
    for (int i = 0; i < list[j].head->char_num; ++i) {
      result[i] = negetable_result[i];
    }
  }

  return 1;
}

int Conjunctable(int j, int result[], char *condition_start) {
  char *start_move = condition_start;
  char *condition_brackets[COMMANDS_WORD_BIG]; ///括号匹配器
  for (int i = 0; i < COMMANDS_WORD_BIG; ++i) { ///初始化括号匹配器
    condition_brackets[i] = NULL;
  }
  int between = 0;
  for (int i = 0; *start_move != '\0'; ++start_move) {
    if (*start_move == '(') {   //左括号处理
      condition_brackets[i] = start_move;
      ++i;
    } else if (*start_move == ')') { //右括号处理
      if (i == 0) { //只有右没有左，语法错误
        return -1;
      } else {
        condition_brackets[i - 1] = NULL; //复原匹配器
        --i;
      }
    } else if (strncmp(start_move, "BETWEEN", 7) == 0) {
      between = 1;
    } else if (i == 0 && strncmp(start_move, "AND", 3) == 0) {
      if (between) {
        between = 0;
      } else {
        break;
      }
    }
  }

  char *last_end = start_move - 1, *next_start = start_move + 3;
  char last_start[COMMANDS_WORD_BIG];
  memset(last_start, 0, sizeof last_start);
  strncpy(last_start, condition_start, last_end - condition_start + 1);
  int conjunctable_result1[COMMANDS_NUM_BIG] = {0};
  int flag1 = Negetable(j, conjunctable_result1, last_start);
  if (flag1 != 1) return flag1;
  if (*start_move != '\0') {
    int conjunctable_result2[COMMANDS_NUM_BIG] = {0};
    int flag2 = Conjunctable(j, conjunctable_result2, next_start);
    if (flag2 != 1) return flag2;
    for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
      if (conjunctable_result1[i] == 1 && conjunctable_result2[i] == 1) result[i] = 1;
    }
  } else {
    for (int i = 0; i < list[j].head->char_num; ++i) {
      if (conjunctable_result1[i] == 1) result[i] = 1;
    }
  }

  return 1;
}

int Condition(int j, int result[], char *condition_start) { //此处j为this_list return -1 语法错误 0 执行错误
  char *start_move = condition_start; ///开始走
  if (FindWord(condition_start, ';') == strlen(condition_start) - 1) {
    *(condition_start + strlen(condition_start) - 1) = '\0';//去掉分号
  }
  char *condition_brackets[COMMANDS_WORD_BIG]; ///括号匹配器
  for (int i = 0; i < COMMANDS_WORD_BIG; ++i) { ///初始化括号匹配器
    condition_brackets[i] = NULL;
  }

  for (int i = 0; *start_move != '\0'; ++start_move) {
    if (*start_move == '(') {   //左括号处理
      condition_brackets[i] = start_move;
      ++i;
    } else if (*start_move == ')') { //右括号处理
      if (i == 0) { //只有右没有左，语法错误
        return -1;
      } else {
        condition_brackets[i - 1] = NULL; //复原匹配器
        --i;
      }
    } else if (i == 0 && strncmp(start_move, "OR", 2) == 0) {
      break;
    }
  }

  char *last_end = start_move - 1, *next_start = start_move + 2;
  char last_start[COMMANDS_WORD_BIG];
  memset(last_start, 0, sizeof last_start);
  strncpy(last_start, condition_start, last_end - condition_start + 1);
  int condition_result1[COMMANDS_NUM_BIG] = {0};
  int flag1 = Conjunctable(j, condition_result1, last_start);
  if (flag1 != 1) return flag1;
  if (*next_start != '\0') {
    int condition_result2[COMMANDS_NUM_BIG] = {0};
    int flag2 = Condition(j, condition_result2, next_start);
    if (flag2 != 1) return flag2;
    for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
      if (condition_result1[i] == 1 || condition_result2[i] == 1) result[i] = 1;
    }
  } else {
    for (int i = 0; i < list[j].head->char_num; ++i) {
      if (condition_result1[i] == 1) result[i] = 1;
    }
  }
  return 1;

}

int Delete(int *count, int list_id, int delete_index[]) {
  bool return0 = false;
  if (strcmp(strtok(NULL, " "), "FROM") != 0) return -1; //不是FROM 语法错误
  char *table_name = strtok(NULL, " "); //找到table_name
  if (!CheckName(table_name)) return -1;
  int this_list = -1;
  for (int i = 0; i < list_id; ++i) {
    if (strcmp(table_name, list[i].table_name) == 0) {
      this_list = i;
      break;
    }
  }
  if (this_list == -1) {
    return0 = true;  ///找到所在表格
    this_list = 0;
  }
  char *where = strtok(NULL, " ");
  if (strcmp(where, "WHERE") != 0) return -1;
  char *condition_start = where + 6;
  while (*condition_start == ' ') {
    ++condition_start;
    if (*condition_start == '\0') return -1;
  }
  int condition_succeed = 1;
  int index[COMMANDS_NUM_BIG] = {0};
  condition_succeed = Condition(this_list, index, condition_start);
  if (condition_succeed == -1)return -1;
  if (condition_succeed == 0) return 0;
  for (int i = 0; i < list[this_list].head->char_num; ++i) {
    delete_index[i] = index[i];
    if (index[i] == 1) ++(*count);
  }

  if(return0) return 0 ;
  Node *now = list[this_list].head;   ///这里开始进行删除操作
  while (now != NULL) {
    for (int i = 0; i < now->char_num; ++i) {
      if (index[i] == 1) {
        free(now->strings[i]);
        now->strings[i] = NULL;
      }
    }
    for (int count_tem = 0; count_tem < *count;) {
      for (int i = 0; i < now->char_num; ++i) {
        if (now->strings[i] == NULL) {
          for (int j = i; j < (now->char_num - 1); ++j) {
            free(now->strings[j]);
            now->strings[j] = malloc(COMMANDS_WORD_BIG * sizeof(char));
            if (now->strings[j + 1] == NULL) {
              now->strings[j] = NULL;
            } else {
              strcpy(now->strings[j], now->strings[j + 1]);
            }
          }
          free(now->strings[now->char_num - 1]);
          now->strings[now->char_num - 1] = NULL;
        }
        ++count_tem, --(now->char_num);
        break;
      }
    }

    now = now->next;
  }
  if(return0) return 1;
  return 1;
}

int UpdateDecideType(char *value1, int *sign) { ///找到字符串类型,顺便取掉字符串外引号
  int num1 = FindWord(value1, '\'');
  if (num1 == -1) { ///数字情况（无引号，疑似数字）
    for (int i = 0; i < strlen(value1); ++i) {
      if (!isdigit(*(value1 + i))) {
        *sign = -1;
        return -1;
      }
    }

    if (*sign == 1) {
      return 1;
    } else {
      return -1;
    }

  } else if (num1 == 0) {
    while (1 == 1) {
      int num2 = -1;
      for(int i = 1 ; i < strlen(value1); ++i){
        if(*(value1 + i) == '\'' && *(value1 + i - 1) != '\\'){
          num2 = i;
          break;
        }
      }
      if (num2 != -1) {
        if(*(value1 + num2 + 1) != '\0') return -1;
        return 0;
      } else {
        char *after = strtok(NULL, " ");
        if (after == NULL) {
          *sign = -1;
          return -1;
        }
        *(value1 + strlen(value1)) = ' ';
        strcat(value1, after);
      }
    }

  } else {
    *sign = -1;
    return -1;
  }

}

int Update(int list_id, int *count) {
  char *table_name = strtok(NULL, " ");
  if(!CheckName(table_name)) return -1;
  int this_list = -1;
  for (int i = 0; i < list_id; ++i) {
    if (strcmp(list[i].table_name, table_name) == 0) {
      this_list = i;
      break;
    }
  } ///定下在哪一个表中
  if (this_list == -1) return 0;
  char *set = strtok(NULL, " ");
  if (strcmp(set, "SET") != 0) return -1;
  *(set + 3) = ' ';
  char *where_start = strstr(set, "WHERE"); ///暂未考虑WHERE在字符串内情况
  if (where_start == NULL) return -1;
  *(where_start - 1) = '\0';
  char *condition_start = where_start + 6;
  while (*condition_start == ' ') ++condition_start; ///找到条件子句开始位置
  int update_index[COMMANDS_NUM_BIG] = {0};
  int update_succeed = Condition(this_list, update_index, condition_start); ///拿到需要更新的char_num
  if (update_succeed != 1) return update_succeed;
  set += 3; ///set后开始更新值
  char start_update[COMMANDS_WORD_BIG];///更新语句处理处
  strcpy(start_update, set);
  //策略：先检查，检查成功再更新
  int end = 0, flag = 1;
  while (!end) {
    char *name = strtok(set, " ");
    if (name == NULL) return -1;
    if(!CheckName(name)) return -1;
    Node *now = list[this_list].head;
    while (now != NULL) {
      if (strcmp(now->name, name) == 0) break;
      now = now->next;
    }
    if (now == NULL) return 0; //至此确认下在哪个结构体中
    char *operator = strtok(NULL, " ");
    if (operator == NULL) return -1;
    if (strcmp(operator, "=") != 0) return -1; //至此确认等号

    char *value = strtok(NULL, " ");
    if (value == NULL) return -1;
    int comma = 0;
    /////////
    if (FindWord(value, ',') != -1) {
      comma = 1;
      *(value + FindWord(value, ',')) = '\0';
    } else {
      if(strtok(NULL, " ") != NULL) return -1;
      end = 1;
    }

    int type = UpdateDecideType(value, &flag);///////////
    if(flag != 1) return flag;
    if (type != now->IorC) return 0;
    char *strings_copy[COMMANDS_NUM_BIG];
    for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
      strings_copy[i] = now->strings[i];
    }

    for (int i = 0; i < now->char_num; ++i) { ///此处模仿出改造后的strings
      if (update_index[i] == 1) {
        strings_copy[i] = value;
        if (now->IorC == 0 && strlen(value) > (now->strings_len + 2)) return 0; //检查字符串长度
        if (now->notnull && strcmp(value, "NULL") == 0) return 0; //检查null
      }
    }
    if (now->unique) {
      for (int i = 0; i < now->char_num; ++i) { //检查unique
        for (int j = 0; j < i; ++j) {
          if (strcmp(strings_copy[j], strings_copy[i]) == 0) return 0;
        }
      }
    }
  }
  end = 0;
  while (!end) {
    char *name = strtok(start_update, " ");
    if (name == NULL) return -1;
    if(!CheckName(name)) return -1;
    Node *now = list[this_list].head;
    while (now != NULL) {
      if (strcmp(now->name, name) == 0) break;
      now = now->next;
    }
    if (now == NULL) return 0;
    char *operator = strtok(NULL, " ");
    if (operator == NULL) return -1;
    if (strcmp(operator, "=") != 0) return -1;
    char *value = strtok(NULL, " ");
    if (value == NULL) return -1;
    int comma = 0;
    if (FindWord(value, ',') != -1) {
      comma = 1;
      *(value + FindWord(value, ',')) = '\0';
    } else {
      if(strtok(NULL, " ") != NULL)return -1;
      end = 1;
    }

    int type = UpdateDecideType(value, &flag);
    if(flag != 1) return flag;
    if (type != now->IorC) return 0;

    for (int i = 0; i < now->char_num; ++i) {
      if (update_index[i] == 1) {
        strcpy(now->strings[i], value);
        ++(*count);
      }
    }

  }

  return 1;
}

int ID_sort_primary(int this_list, int id_amount, int *ptr_id[]) {
  Node *now = list[this_list].head;
  while (now != NULL){
    if(now->primary) break;
    now = now->next;
  }
  char *array[COMMANDS_NUM_BIG];
  memset(array, 0, sizeof array);
  for (int i = 0; i < id_amount; ++i) {
    array[i] = malloc(40 * sizeof(char));
    strcpy(array[i], now->strings[*ptr_id[i]]);
  }
  if (now->IorC) { //排序部分
    for (int j = 0; j < id_amount; ++j) {
      int min = atoi(array[j]), min_index = j;
      for (int i = j; i < id_amount; ++i) {
        if (atoi(array[i]) < min) {
          min = atoi(array[i]);
          min_index = i;
        }
      }
      char *tem = array[j];
      array[j] = array[min_index];
      array[min_index] = tem;

      int temp = *ptr_id[j];
      *ptr_id[j] = *ptr_id[min_index];
      *ptr_id[min_index] = temp;

    }
  } else { //排序部分
    for (int j = 0; j < id_amount; ++j) {
      char *min_str = array[j];
      int min_index = j;
      for (int i = j; i < id_amount; ++i) {
        if (strcmp(array[i], min_str) < 0) {
          min_str = array[i];
          min_index = i;
        }
      }

      char *tem = array[j];
      array[j] = array[min_index];
      array[min_index] = tem;

      int temp = *ptr_id[j];
      *ptr_id[j] = *ptr_id[min_index];
      *ptr_id[min_index] = temp;
    }
  }
  for (int i = 0; i < id_amount; ++i) {
    free(array[i]);
  }
  return 1;

}

int ID_sort(int *ptr_id[COMMANDS_NUM_BIG], int id_amount, char *sort_name, int this_list) {
  char *next = NULL;
  int end = 0, AorD = 1;
  if (FindWordBack(sort_name, ';') == strlen(sort_name) - 1) {
    *(sort_name + FindWordBack(sort_name, ';')) = '\0';
    end = 1;
  }
  if (!CheckName(sort_name)) return -1;
  if (!end) {
    if (FindWordBack(sort_name, ',') == strlen(sort_name) - 1) {//查sort_name逗号
      *(sort_name + FindWordBack(sort_name, ',')) = '\0';
      next = strtok(NULL, " ");
    } else { //sort_name查不到逗号，逗号应该在AD里
      char *AD = strtok(NULL, " ");
      if (FindWordBack(AD, ';') == strlen(AD) - 1) { //查分号，查到next定为NULL
        *(AD + FindWordBack(AD, ';')) = '\0';
        end = 1;
      } else { //查不到分号，查逗号
        if (FindWordBack(AD, ',') == strlen(AD) - 1) {//查逗号，再查不到直接报错
          *(AD + FindWordBack(AD, ',')) = '\0';
          next = strtok(NULL, " ");
        } else {
          return -1;
        }
      }
      if (strcmp(AD, "DESC") == 0) {
        AorD = 0;
      } else if (strcmp(AD, "ASC") == 0) {
        AorD = 1;
      } else {
        return -1;
      }
    }
  }

  Node *now = GetSameName(this_list, sort_name);
  if (now == NULL) return 0;
  char *array[COMMANDS_NUM_BIG];
  for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
    array[i] = NULL;
  }

  for (int i = 0; i < id_amount; ++i) {
    array[i] = malloc(40 * sizeof(char));
    strcpy(array[i], now->strings[*ptr_id[i]]);
  }

  if (now->IorC) { //排序部分
    for (int j = 0; j < id_amount; ++j) {
      int min = atoi(array[j]), min_index = j;
      for (int i = j; i < id_amount; ++i) {
        if (atoi(array[i]) < min) {
          min = atoi(array[i]);
          min_index = i;
        }
      }
      char *tem = array[j];
      array[j] = array[min_index];
      array[min_index] = tem;

      int temp = *ptr_id[j];
      *ptr_id[j] = *ptr_id[min_index];
      *ptr_id[min_index] = temp;

    }
  } else { //排序部分
    for (int j = 0; j < id_amount; ++j) {
      char *min_str = array[j];
      int min_index = j;
      for (int i = j; i < id_amount; ++i) {
        if (strcmp(array[i], min_str) < 0) {
          min_str = array[i];
          min_index = i;
        }
      }

      char *tem = array[j];
      array[j] = array[min_index];
      array[min_index] = tem;

      int temp = *ptr_id[j];
      *ptr_id[j] = *ptr_id[min_index];
      *ptr_id[min_index] = temp;
    }
  }

  int null_num = 0;
  for (int i = 0; i < id_amount - null_num; ++i) {
    if (strcmp(array[i], "NULL") == 0) {
      int tem = *ptr_id[i];
      for (int j = i; j < id_amount - null_num - 1; ++j) {
        strcpy(array[j], array[j + 1]);
        *ptr_id[j] = *ptr_id[j + 1];
      }

      strcpy(array[id_amount - null_num - 1], "NULL");
      *ptr_id[id_amount - null_num - 1] = tem;
      ++null_num, --i;
    }
  }
//本次排序完成，考虑下一次排序
  int *ptr_id_next[COMMANDS_NUM_BIG];
  for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
    ptr_id_next[i] = NULL;
  } //下一次排序用的id地址
  for (int i = 0, j = 0; i < id_amount; ++i) {
    ptr_id_next[j] = ptr_id[i];
    if ((i != id_amount - 1 && strcmp(array[i], array[i + 1]) == 0) ||
        id_amount != 1 && i == id_amount - 1 && strcmp(array[i], array[i - 1]) == 0) {
      ++j;
    } else if (j != 0 && (strcmp(array[i], array[i + 1]) != 0 || i == id_amount - 1)) {
      int nag = 1;
      if (end) {
        nag = ID_sort_primary(this_list, j + 1, ptr_id_next);
      } else {
        nag = ID_sort(ptr_id_next, j + 1, next, this_list);
      }
      if (nag != 1) return nag;
      j = 0;
    }
  }
  if (!AorD) { ///判定倒序
    for (int i = 0; i < (id_amount - null_num) / 2; ++i) {
      int tem = *ptr_id[i];
      *ptr_id[i] = *ptr_id[id_amount - i - 1];
      *ptr_id[id_amount - i - 1] = tem;
    }
  }

  for (int i = 0; i < id_amount; ++i) {
    free(array[i]);
  }

  return 1;
}

int Select(int list_id) {
  int select_index[COMMANDS_NUM_BIG] = {0};
  int order = 1, where = 1;//是否有order by 和 where
  char *sort_value = strtok(NULL, " ");
  *(sort_value + strlen(sort_value)) = ' ';
  char *order_start = strstr(sort_value, "ORDER");//开始排序位置
  if (order_start == NULL) {
    order = 0;
  } else {
    *(order_start - 1) = '\0';
  }

  char *where_start = strstr(sort_value, "WHERE");
  if (where_start == NULL) {
    where = 0;
  } else {
    *(where_start - 1) = '\0';
  }

  char *from = strstr(sort_value, "FROM");
  if (from == NULL) {
    return -1;
  } else {
    *(from - 1) = '\0';
  }


  ///至此完成分割
  // 下面开始处理
  if (strcmp(strtok(from, " "), "FROM") != 0) return -1;
  char *table_name = strtok(NULL, " ");
  ///这里加一步判断table_name后面有没有分号
  if (FindWord(table_name, ';') == strlen(table_name) - 1) {
    *(table_name + FindWord(table_name, ';')) = '\0';
  }
  if(strtok(NULL, " ") != NULL) return -1;

  int this_list = -1;
  for (int i = 0; i < list_id; ++i) {
    if (strcmp(list[i].table_name, table_name) == 0) {
      this_list = i;
      break;
    }
  }
  if (this_list == -1) return 0;
  if (where) { ///条件子句处理
    if (strcmp(strtok(where_start, " "), "WHERE") != 0) return -1;
    char *condition_start = strtok(NULL, " ");
    int flag = Condition(this_list, select_index, condition_start);
  } else {
    for(int i = 0 ; i < list[this_list].head->char_num; ++i){
      select_index[i] = 1;
    }
  }

  int id[COMMANDS_NUM_BIG] = {0};///array此处为可排序的char_num下标
  int id_amount = 0;
  for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
    if (select_index[i] == 1) {
      id[id_amount] = i;
      ++id_amount;
    }
  }
  int *ptr_id[COMMANDS_NUM_BIG];
  for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
    ptr_id[i] = NULL;
  }

  for (int i = 0; i < id_amount; ++i) {
    ptr_id[i] = &id[i];
  }
  if (order) {
    if (strcmp(strtok(order_start, " "), "ORDER") != 0) return -1;
    if (strcmp(strtok(NULL, " "), "BY") != 0) return -1;
    char *sort_name = strtok(NULL, " ");

    int judge = ID_sort(ptr_id, id_amount, sort_name, this_list);
    if (judge != 1) return judge;
  } else {
    int judge = ID_sort_primary(this_list, id_amount, ptr_id);
    if (judge != 1) return judge;
  }

  char *name = strtok(sort_value, " ");
  Node *select_name[COMMANDS_NUM_BIG];
  for (int i = 0; i < COMMANDS_NUM_BIG; ++i) {
    select_name[i] = NULL;
  }
  int select_name_amount = 0;
  if (strcmp(name, "*") == 0) {
    Node *now = list[this_list].head;
    while (now != NULL) {
      select_name[select_name_amount] = now;
      ++select_name_amount;
      now = now->next;
    }
  } else {
    while (1 == 1) {
      int comma = 0;
      if (FindWord(name, ',') == strlen(name) - 1) {
        *(name + FindWord(name, ',')) = '\0';
        comma = 1;
      }
      Node *now = list[this_list].head;
      while (now != NULL) {
        if (strcmp(now->name, name) == 0) break;
        now = now->next;
      }
      if (now == NULL) return 0;
      select_name[select_name_amount] = now;
      ++select_name_amount;

      if (comma) {
        name = strtok(NULL, " ");
      } else {
        char *judge = strtok(NULL, " ");
        if (judge != NULL) {
          return -1;
        } else {
          break;
        }
      }
    }
  }
  printf("%d RECORDS FOUND\n", id_amount);
  if (id_amount != 0) {
    for (int i = 0; i < select_name_amount; ++i) {
      printf("%s\t", select_name[i]->name);
    }
    printf("\n");

    for (int j = 0; j < id_amount; ++j) {
      for (int i = 0; i < select_name_amount; ++i) {
        printf("%s\t", select_name[i]->strings[id[j]]);
      }
      printf("\n");
    }
  }
  return 1;
}