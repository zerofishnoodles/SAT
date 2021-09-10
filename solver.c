//
// Created by Ray on 2021/3/1.
//

#include "head.h"

status dpll(ClauseNode* root, int** truth_table_p, int mode)
{
    struct ClauseNode *unit_clause=NULL;
    while (unit_clause = select_unit_clause(root, truth_table_p)){
        clause_simplify(&root, unit_clause);
        if (is_clause_empty(root)) return TURE;
        else if (null_clause_exist(root)) return FALSE;
    }

    int var;
//    printf("start\n");
//    for(int i=1;i<=num_var;i++) printf("%d ", (*truth_table_p)[i]);
//    show_data(root);
    var = select_var(root, mode, *truth_table_p);
    int a[2] = {1,var};
//    printf("\nselect var: %d\n", var);
    if (dpll(add_clause(copy_root(root), a), truth_table_p, mode)) return TURE;
//    show_data(root);
    a[1] = -var;
    return dpll(add_clause(copy_root(root), a), truth_table_p, mode);
}

/**
 * 选取单子句并设置为真
 * @param root
 * @param truth_table_p
 * @return 返回单子句
 */
struct ClauseNode* select_unit_clause(struct ClauseNode *root, int** truth_table_p)
{
//    show_data(root);
    struct ClauseNode* tail = root->next;
    int *truth_table = *truth_table_p;
    while (tail){
        if (tail->length == 1) {
            truth_table[abs(tail->head->data)] = tail->head->data>0 ? 1 : 0;
//            printf("select:%d\n",tail->head->data);
            return tail;
        }
        tail = tail->next;
    }
//    printf("null!\n");
    return NULL;
}

/**
 * simplify the clauses according to the rule
 * @param root_p
 * @param unit_clause
 */
void clause_simplify(struct ClauseNode** root_p, struct ClauseNode* unit_clause)
{
    struct ClauseNode* root = *root_p;
//    printf("before:\n"); show_data(root);
    struct ClauseNode* pre_tail = root;
    struct ClauseNode* tail = root->next;
    int var = unit_clause->head->data;
    int flag;
    while (tail){  //node->node->...->pre_tail->tail
        flag = FALSE; //record if destroy this clause
        struct LiteralNode* cur = tail->head;

        // the first one should be treat differently
        if (cur && cur->data == -var){
            if (cur->next) {
                while (cur && cur->data == -var){
                    tail->head = cur->next;
                    free(cur);cur = NULL;
                    cur = tail->head;
                    tail->length--;
                }
            }
            else {
                tail->head = NULL;
                tail->length = 0;  // null clause
                free(cur); cur = NULL;
                pre_tail = pre_tail->next;
                tail = pre_tail->next;
                continue;
            }
        }
        //the rest one
        while (cur){
            int flag_cur = FALSE;
            if (cur->data == var){ //destroy this clause
                pre_tail->next = tail->next;
                free(tail);tail = NULL;
                tail = pre_tail->next;
                flag = TURE;
                break;
            }
            if (cur->next == NULL) break;
            else{
                if (cur->next->data == -var){
                    //delete the literal
                    if (cur->next->next){
                        struct LiteralNode* temp;
                        temp = cur->next;
                        cur->next = cur->next->next;
                        tail->length--;
                        free(temp); temp = NULL;
                        continue;
                    } else {
                        free(cur->next);
                        cur->next = NULL;
                        flag_cur = TURE;
                        tail->length--;
                        break;
                    }
                }
            }
            cur = cur->next;
        }
        if (tail == NULL) break;
        if (!flag || tail->length==0 ) {
            pre_tail = tail;
            tail = tail->next;
        }

    }
//    printf("after:\n"); show_data(root);
}

/**
 * judge if there is no clause in the space
 * @param root
 * @return
 */
status is_clause_empty(struct ClauseNode *root)
{
    if (root->next == NULL) return TURE;
    else return FALSE;
}

/**
 * judge if there is null clause in the clauses
 * @param root
 * @return
 */
status null_clause_exist(struct ClauseNode *root)
{
    struct ClauseNode* tail;
    tail = root;
    while (tail->next){
        if (tail->next->length == 0) return TURE;
        tail = tail->next;
    }
    return FALSE;
}


int select_var(struct ClauseNode *root, int mode, int *truth_table)
{

    if (mode == 1) {
        struct ClauseNode *min_clause = get_min_clause(root);
        return min_clause->head->data;
    }
    if (mode == 2) {
        struct ClauseNode *min_clause = get_min_clause(root);
        struct ClauseNode *tail = root->next;
        int *count_table = (int *)malloc(sizeof(int) * 2* (num_var+1));
        memset(count_table, 0, sizeof(int) * 2* (num_var+1));
        while (tail){
            if (tail->length == min_clause->length){
                struct LiteralNode *lit_tail = tail->head;
                while (lit_tail){
                    int number = lit_tail->data > 0 ? lit_tail->data : abs(lit_tail->data)+num_var;
                    count_table[number]++;
//                    printf("%d ", lit_tail->data);
                    lit_tail = lit_tail->next;
                }
//                printf("\n");
            }
            tail = tail->next;
        }
        int max = 0;
        int max_var = 0;
//        for(int i=1;i<=2*num_var;i++) printf("%d ", count_table[i]);
        for(int i=1;i<=2*num_var;i++){
            if (count_table[i]>max){
                max = count_table[i];
                max_var = i > num_var ? -(i-num_var) : i;
            }
        }
        return max_var;
    }
    return root->next->head->data;
}

struct ClauseNode* add_clause(struct ClauseNode *root, int *var)
{
    /**
     * add in the head
     */

    struct ClauseNode* new_clause = create_clause(var);
//    new_clause = (struct ClauseNode*)malloc(sizeof(struct ClauseNode));
//    struct LiteralNode* new_literal;
//    new_literal = (struct LiteralNode*)malloc(sizeof(struct LiteralNode));
//    new_literal->data = var;
//    new_literal->next = NULL;
//    new_clause->length = 1;
//    new_clause->head = new_literal;

    new_clause->next = root->next;
    root->next = new_clause;
    return root;
}

struct ClauseNode* copy_root(struct ClauseNode* root)
{
    struct ClauseNode *new_root;
    new_root = (struct ClauseNode*)malloc(sizeof(struct ClauseNode));
    struct ClauseNode *new_tail = new_root, *tail = root->next;
    while (tail){
        struct ClauseNode *new_node;
        new_node = copy_clause(tail);
        new_tail->next = new_node;
        new_tail = new_tail->next;
        tail = tail->next;
    }
    new_tail->next = NULL;
    return new_root;
}

struct ClauseNode* copy_clause(struct ClauseNode* clause)
{
    struct ClauseNode *new_clause;
    new_clause = (struct ClauseNode*)malloc(sizeof(struct ClauseNode));
    new_clause->length = clause->length;
    if (new_clause->length == 0){
        new_clause->head = NULL;
        return new_clause;
    }
    struct LiteralNode *new_clause_head, *new_clause_tail, *head=clause->head, *tail=head->next;
    new_clause_head = (struct LiteralNode*)malloc(sizeof(struct LiteralNode));
    new_clause_head->data = head->data;
    new_clause->head = new_clause_head;
    new_clause_tail = new_clause_head;
    while (tail){
        struct LiteralNode *new_literal;
        new_literal = (struct LiteralNode*)malloc(sizeof(struct LiteralNode));
        new_literal->data = tail->data;
        new_clause_tail->next = new_literal;
        new_clause_tail = new_literal;
        tail = tail->next;
    }
    new_clause_tail->next = NULL;
    return new_clause;
}

/**
 * create clause
 * @param var var[0]为变元个数，var[1:]为变元
 * @return
 */
struct ClauseNode* create_clause(int *var)
{
    struct ClauseNode *new_clause;
    new_clause = (ClauseNode *)malloc(sizeof(ClauseNode));
    new_clause->length = var[0];
    new_clause->next = NULL;
    new_clause->head = NULL;
    if (var[0] == 0) return new_clause;

    struct LiteralNode *lit_head, *lit_tail;
    lit_head = (LiteralNode *)malloc(sizeof(LiteralNode));
    lit_head->data = var[1];
    lit_head->next = NULL;
    lit_tail = lit_head;
    new_clause->head = lit_head;
    for (int i=2;i<=var[0];i++){
        LiteralNode *new_lit;
        new_lit = (LiteralNode *)malloc(sizeof(struct LiteralNode));
        new_lit->data = var[i];
        new_lit->next = NULL;
        lit_tail->next = new_lit;
        lit_tail = new_lit;
    }

    return new_clause;

}
/**
 * 找长度最小的字句
 * @param root
 * @return
 */
struct ClauseNode* get_min_clause(struct ClauseNode *root)
{
    struct ClauseNode *tail;
    tail = root->next;
    struct ClauseNode *min_clause = tail;
    int min_length = 1000000;
    while (tail){
        if(tail->length < min_length){
            min_length = tail->length;
            min_clause = tail;
        }
        tail = tail->next;
    }
    return min_clause;
}
