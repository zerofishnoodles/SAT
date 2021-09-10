//
// Created by Ray on 2021/3/1.
//

#include "head.h"

#define MAX_LINE_SIZE 100
#define MAX_NUM_SIZE 10
#define VAR_INCREMENT 5
#define MAX_VAR_NUM 10

void read_file(char *file_root, struct ClauseNode* root)
{
    /*
     * function: print note, load the cnf to data structure
     * input: file root
     */

    FILE *fp;
    fp = fopen(file_root, "rb");
    if (fp == NULL){
        printf("open file failed!");
        exit(-1);
    }
    char *line;

    // print note
    while (!feof(fp)){
        line = (char *)malloc(sizeof(char) * MAX_LINE_SIZE);
        fgets(line, MAX_LINE_SIZE, fp);
        if (line[0] == 'p') break;
        printf("%s", line);
    }

    //get the info
    char *info[3];  //0:type 1:num_variety 2:num_clause
    int j=2;
    for(int i=0;i<3;i++){
        info[i] = (char *)malloc(sizeof(char) * MAX_NUM_SIZE);
        int k=0;
        while (line[j] != ' ' && line[j] != '\n'){
            info[i][k] = line[j];
            k++;
            j++;
        }
        info[i][k] = '\0';
        j++;
    }
    num_var = atoi(info[1]);
    num_clause = atoi(info[2]);
    root->length = num_clause;
    printf("type:%s\t\tnumber of varieties:%d\t\tnumber of clause:%d\n", info[0], num_var, num_clause);

    // read clause
    //raw data
    raw_cnf_data = (char  **)malloc(sizeof(char *) * num_clause);
    int cnf_count=0;

    struct ClauseNode *temp_clause_tail;
    temp_clause_tail = root;

    while (!feof(fp)){
        int *clause;
        line = (char *)malloc(sizeof(char) * MAX_LINE_SIZE);
        fgets(line, MAX_LINE_SIZE, fp);
        if (feof(fp)) break;
        if (line[0] == 'c') continue;
        clause = clause_parser(line);

        //raw data save
        int length = strlen(line);
        raw_cnf_data[cnf_count] = (char *)malloc(sizeof(char) * (length+2));
        strcpy(raw_cnf_data[cnf_count], line);
        cnf_count++;

        struct ClauseNode *new_clause;
        new_clause = (struct ClauseNode *)malloc(sizeof(struct ClauseNode));
        new_clause->length = clause[0];
        new_clause->head = NULL;

        //build the first literal node
        struct LiteralNode *literal_head, *temp_literal_tail;
        literal_head = (struct LiteralNode *)malloc(sizeof(struct LiteralNode));
        literal_head->data = clause[1];
        literal_head->next = NULL;
        temp_literal_tail = literal_head;
        new_clause->head = literal_head;
        new_clause->length = clause[0];

        //concat the literal
        for (int i=2;i<=clause[0];i++){
            struct LiteralNode *new_literal;
            new_literal = (struct LiteralNode *)malloc(sizeof(struct LiteralNode));
            new_literal->data = clause[i];
            new_literal->next = NULL;
            temp_literal_tail->next = new_literal;
            temp_literal_tail = temp_literal_tail->next;
            temp_literal_tail->next = NULL;
        }

        //concat the clause
        temp_clause_tail->next = new_clause;
        temp_clause_tail = new_clause;
        temp_clause_tail->next = NULL;


    }

}

int *clause_parser(char *clause)
{
    //parse the clause in the file
    //input: clause in str
    //output: number of var in this clause: var[0], the var in clause: var[1:]

    int i=0;
    int *var;
    int num_var_in_clause=0;  //number of var in one clause
    var = (int *)malloc(sizeof(int) * MAX_VAR_NUM);
    int var_array_size = MAX_VAR_NUM;
    while (clause[i] != '0'){
        char *var_str = (char *)malloc(sizeof(char) * MAX_NUM_SIZE);
        int j_var=0;
        while (clause[i] != ' '){
            var_str[j_var] = clause[i];
            i++;j_var++;
        }
        var_str[j_var] = '\0';
        i++;
        var[num_var_in_clause+1] = atoi(var_str);
        num_var_in_clause++;
        if (num_var_in_clause + 1 == var_array_size){
            int *new_var;
            new_var = realloc(var, sizeof(int) * (var_array_size+VAR_INCREMENT));
            if (new_var == NULL){
                printf("extend the variety num in one clause failed\n");
                exit(-1);
            }else{
                var = new_var;
                var_array_size = var_array_size + VAR_INCREMENT;
            }
        }
    }

    var[0] = num_var_in_clause;
    return var;

}


void cnf_parser(struct ClauseNode* root)
{
    char file_root[100];
    printf("please input the file root:");
    scanf("%s", file_root);

    read_file(file_root, root);
    val_dialog();
//    var_count_parser(root);

    int op;
    scanf("%d", &op);
    if (op == 1) val_show(root);
}

//void var_count_parser(struct ClauseNode *root)
//{
//    int *temp_var_count;
//    dif_var_count = 0;
//    int index=0;
//    struct ClauseNode *tail = root->next;
//    var_count = malloc(sizeof(int) * 2 * (num_var+1));
//    memset(var_count, 0, sizeof(int) * 2 * (num_var+1));
////    show_data(root);
//    while (tail){
//        struct LiteralNode *lit_head = tail->head, *lit_tail = lit_head;
//        while (lit_tail){
//            if (lit_tail->data < 0) var_count[abs(lit_tail->data)+num_var]++;
//            else var_count[lit_tail->data]++;
//            lit_tail = lit_tail->next;
//        }
//        tail = tail->next;
//    }
//
//    for (int i=1;i<=2*num_var;i++){
//        if (var_count[i] != 0) dif_var_count++;
//    }
//    temp_var_count = (int *)malloc(sizeof(int) * (dif_var_count+1));
//    memset(temp_var_count, 0, sizeof(int) *  (dif_var_count+1));
//
//    for(int i=0;i<dif_var_count;i++){
//        int max = var_count[1];
//        int max_var = 1;
//        for(int j=1;j<=2*num_var;j++){
//            if (var_count[j] > max){
//                max = var_count[j];
//                max_var = j;
//            }
//        }
//        var_count[max_var] = 0;
//        temp_var_count[index] = max_var > num_var ? -(max_var-num_var) : max_var;
//        index++;
//    }
//    free(var_count);
//    var_count = temp_var_count;
//
////    for(int i=0;i<index;i++){
////        printf("%d ", var_count[i]);
////    }
//
//}

