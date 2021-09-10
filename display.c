//
// Created by Ray on 2021/2/28.
//

#include "head.h"


void menu(void)
{
    printf("----------------MENU------------------\n");
    printf("1. SAT          2. sudoku           0. exit\n");
    printf("please enter your choice: ");
}

void val_dialog(void)
{
    printf("cnf parse succeed!\n");
    printf("would you want to validate the process?\n");
    printf("1.YES\t\t2.NO\n");
    printf("please enter your choice:");
}

void show_data(struct ClauseNode* root){
    printf("-----------------------show_data--------------------\n");
    struct ClauseNode *clause_tail;
    clause_tail = root->next;
    while (clause_tail){
        LiteralNode *literal_tail;
        literal_tail = clause_tail->head;
        printf("num_var: %d\t", clause_tail->length);
        while (literal_tail){
            printf("%d ", literal_tail->data);
            literal_tail = literal_tail->next;
        }
        printf("\n");
        clause_tail = clause_tail->next;
    }
}

void val_show(struct ClauseNode* root)
{
    struct ClauseNode *clause_tail;
    clause_tail = root->next;
    int cnf_count=0;
    while (clause_tail){
        printf("%d. raw: %s", cnf_count+1, raw_cnf_data[cnf_count]);
        printf("%d. load: ", cnf_count+1);
        cnf_count++;
        LiteralNode *literal_tail;
        literal_tail = clause_tail->head;
        while (literal_tail){
            printf("%d ", literal_tail->data);
            literal_tail = literal_tail->next;
        }
        printf("\n\n");
        clause_tail = clause_tail->next;
    }
    printf("validate process finished!\n");
}

void sat_result(status flag, int *truth_table)
{
    if (flag){
        printf("It is satisfiable!\nThe truth table:\n");
        for (int i=1;i<=num_var;i++){
            printf("var %d: %d\n", i, truth_table[i]);
//            printf("%d ", truth_table[i]);
        }
    }else{
        printf("It is unsatisfiable!\n");
    }
}

int mode_select(void)
{
    printf("\n--------------MODE-------------\n");
    printf("base mode 0: select the first var\n");
    printf("mode 1: select the first mode of the shortest sentence\n");
//    printf("mode 2: select the most var among the min clause\n");
    printf("please enter the optimized mode you want to use:");
    int mode;
    scanf("%d", &mode);
    return mode;

}

status val_res(int *truth_table, struct ClauseNode *root){
    int a[2]={1,1};
    for(int i=1;i<=num_var;i++) {
        if (truth_table[i] == 1) {
            a[1] = i;
            add_clause(root, a);
        }
        else {
            a[1] = -i;
            add_clause(root, a);
        }
    }
    struct ClauseNode *unit_clause;
    int *_;
    _ = (int *)malloc(sizeof(int) * (num_var+1));
    while (unit_clause = select_unit_clause(root, &_)){
        clause_simplify(&root, unit_clause);
    }
    if (is_clause_empty(root)) return TURE;
    else return FALSE;
}

int sudoku_load_display(void)
{
    int mode;
    printf("----------------sudoku loader----------------\n");
    printf("mode 1: load from existed sudoku\n");
    printf("mode 2: load from random sudoku\n");
    printf("please enter the mode of the sudoku loader:");
    scanf("%d", &mode);
    return mode;
}

void sudoku_result(int *truth_table, int **board, int order)
{
    printf("-----------------sudoku result----------------\n");
    for(int i=1;i<=order;i++){
        for(int j=1;j<=order;j++){
            board[i][j] = truth_table[(i-1)*order+j];
        }
    }
    show_board(board, order);
}
void show_board(int **board, int order)
{
    printf("-----------------show board------------------\n");
    for(int i=1;i<=order;i++){
        for(int j=1;j<=order;j++){
            if (board[i][j] == -1) {
                printf(".");
                continue;
            }
            printf("%d",board[i][j]);
        }
        printf("\n");
    }
}

void save_cnf_file(ClauseNode *root, int **board, int order)
{
    printf("----------------save cnf file------------------\n");
    FILE *fp;
    char file_root[100];
    printf("please input the file root:");
    scanf("%s", file_root);
    fp = fopen(file_root, "w");
    if (fp == NULL){
        printf("open file failed!\n");
        exit(-1);
    }
    //get the info
    int clause_num = get_clause_num(root);
    int var_num = cur_add_num-1;

    //print information
    fprintf(fp, "c  created by ray\n");
    fprintf(fp, "c\nc\nc\n");
    for(int i=1;i<=order;i++){
        fputc('c',fp); fputc('\t', fp); fputc('\t', fp);
        for(int j=1;j<=order;j++){
            if (board[i][j]==-1) fputc('.', fp);
            if (board[i][j] == 0) fputc('0', fp);
            if (board[i][j] == 1) fputc('1', fp);
        }
        fputc('\n', fp);
    }
    fprintf(fp, "c\nc\nc\n");
    fprintf(fp, "p cnf %d %d\n", var_num, clause_num);

    //print data
    struct ClauseNode *clause_tail;
    clause_tail = root->next;
    while (clause_tail){
        LiteralNode *literal_tail;
        literal_tail = clause_tail->head;
        while (literal_tail){
            fprintf(fp, "%d ", literal_tail->data);
            literal_tail = literal_tail->next;
        }
        fprintf(fp, "0\n");
        clause_tail = clause_tail->next;
    }

    fclose(fp);
}

int get_clause_num(ClauseNode *root)
{
    struct ClauseNode *tail = root->next;
    int clause_count = 0;
    while (tail) {
        clause_count++;
        tail = tail->next;
    }
    return clause_count;
}

void result(int flag, int *truth_table, int time)
{
    char file_root[100];
    printf("please input the file root of te result:");
    scanf("%s", file_root);
    FILE *fp;
    fp = fopen(file_root, "w");
    if (fp == NULL){
        printf("open file failed !");
        exit(-1);
    }

    fprintf(fp, "s %d\nv ", flag);
    if (flag != 0) {
        for (int i = 1; i <= num_var; i++) {
            if (truth_table[i] == 0) fprintf(fp, "%d ", -i);
            else fprintf(fp, "%d ", i);
        }
    }
    fprintf(fp, "\nt %d", time);
    fclose(fp);
}

