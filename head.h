//
// Created by Ray on 2021/3/1.
//

#ifndef SAT_HEAD_H
#define SAT_HEAD_H

#include "stdio.h"
#include "math.h"
#include "malloc.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

typedef int status;
#define TURE 1
#define FALSE 0

/*------------------data_structure---------------------*/
typedef struct LiteralNode{
    int data;
    struct LiteralNode *next;
}LiteralNode;

typedef struct ClauseNode{
    struct LiteralNode *head;
    struct ClauseNode *next;
    int length;
}ClauseNode;

//typedef struct VarCount {
//    int var;
//    int count;
//}VarCount;

/*------------------global variable---------------------*/
int num_var;
int num_clause;
char **raw_cnf_data;
int cur_var_count;
int *var_count;
int dif_var_count;
int cur_add_num;


/*------------------display----------------------------*/
void menu(void);
void sat_result(status flag, int* truth_table);
void val_dialog(void);
void show_data(struct ClauseNode* root);
void val_show(struct ClauseNode* root);
int mode_select(void);
status val_res(int *truth_table, struct ClauseNode *root);
int sudoku_load_display(void);
void show_board(int **board, int order);
void save_cnf_file(ClauseNode *root, int **board, int order);
int get_clause_num(ClauseNode *root);
void result(int flag, int *truth_table, int time);



/*------------------cnf_parser-------------------------*/
void read_file(char *file_root, struct ClauseNode* root);
int *clause_parser(char *clause);
void cnf_parser(struct ClauseNode *root);
void var_count_parser(struct ClauseNode *root);


/*------------------solver-----------------------------*/
status dpll(struct ClauseNode *root, int **truth_table_p, int mode);
struct ClauseNode* select_unit_clause(struct ClauseNode* root, int** truth_table_p);
void clause_simplify(struct ClauseNode **root_p, struct ClauseNode *unit_clause);
status is_clause_empty(struct ClauseNode *root);
status null_clause_exist(struct ClauseNode *root);
int select_var(struct ClauseNode *root, int mode, int *truth_table);
struct ClauseNode* add_clause(struct ClauseNode *root, int *var);
void destroy_clause(struct ClauseNode** root_p, int var);
struct ClauseNode* copy_root(struct ClauseNode* root);
struct ClauseNode* copy_clause(struct ClauseNode*);
void sort_var_count(void);
int mom(struct ClauseNode *root);
struct ClauseNode* create_clause(int *var);
struct ClauseNode* get_min_clause(struct ClauseNode *root);


/*--------------------------sudoku-------------------------*/
void sudoku_parser(int **board, struct ClauseNode *root, int order);
void constraint2(ClauseNode *root, int order, int start, int step);
void dfs_cons2(struct ClauseNode* root, int start, int end, int step, int k, int index, int *res);
void tseytin(ClauseNode *root, int r1, int r2, int order,  int mode);
int** board_loader(int order, int mode);
int **board_generator(int **raw_board, int order);
status is_follow_restrict(int **raw_board, int order);
status is_unique(int **raw_board, int order, int r, int c);
void sudoku_result(int *truth_table, int **board, int order);
void terminal_generator(int **board, int order);
status is_board_legal(int **board, int order);
void sudoku(struct ClauseNode* root);
void play(int **board, int order, int *truth_table);
status match_res(int **board, int **res, int order, int *truth_table);



#endif //SAT_HEAD_H
