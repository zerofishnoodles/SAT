//
// Created by Ray on 2021/3/7.
//

#include "head.h"
#define MAX_LINE_SIZE 100
#define LOWER_BOUND_PROB 0.4
#define GEN_NUM_PROB 0.1
#define MAX_TRUTH_NUM 30000

/**
 * 根据三个约束生成字句
 * @param board
 * @param root
 * @param order
 */
void sudoku_parser(int **board, ClauseNode *root, int order)
{
    //add given clause
    int unit[2] = {1,1};
    for(int i=1; i<=order; i++){
        for (int j=1;j<=order;j++){
            if (board[i][j] == 0) {
                unit[1] = -((i-1)*order+j);
                add_clause(root, unit);
            }
            if (board[i][j] == 1) {
                unit[1] = (i-1)*order+j;
                add_clause(root, unit);
            }
        }
    }
//    show_data(root);

    //constraint 1
    //row
    int tri[4] = {3,1,1,1};
    for(int i=1;i<=order;i++){
        for(int j=1;j<=order-2;j++){
            int seq = (i-1)*order+j;
            tri[1] = seq; tri[2] = seq+1; tri[3] = seq+2;
            add_clause(root, tri);
            tri[1] = -seq; tri[2] = -(seq+1); tri[3] = -(seq+2);
            add_clause(root, tri);
        }
    }
    //col
    for(int j=1;j<=order;j++){
        for(int i=1;i<=order-2;i++){
            int seq = (i-1)*order+j;
            tri[1] = seq; tri[2] = seq+order; tri[3] = seq+2*order;
            add_clause(root, tri);
            tri[1] = -seq; tri[2] = -(seq+order); tri[3] = -(seq+2*order);
            add_clause(root, tri);
        }
    }

//    show_data(root);

    //constraint 2
    //row
    for(int i=1;i<=order;i++){
        constraint2(root, order, (i-1)*order+1, 1);
    }
    //col
    for(int j=1;j<=order;j++){
        constraint2(root, order, j, order);
    }

//    show_data(root);
    //constraint3
    cur_add_num = order * order + 1;
    for(int i=1;i<=order-1;i++){
        for(int j=i+1;j<=order;j++){
            tseytin(root,i,j,order,1);
            tseytin(root, i, j, order, 2);
        }
    }

//    show_data(root);
}

void constraint2(struct ClauseNode *root, int order, int start, int step)
{
    int end = start + step*order -1;
    int k = order/2+1;
    int *res;
    res = (int *)malloc(sizeof(int) * (k+1));
    res[0] = k;
    dfs_cons2(root, start, end, step, k, 1, res);
}

void dfs_cons2(struct ClauseNode* root, int start, int end, int step, int k, int index, int *res){
    if (index-1 == k){
        add_clause(root, res);
        int *res_neg = (int *)malloc(sizeof(int) * (k+1));
        res_neg[0] = k;
        for(int i=1;i<=k;i++) res_neg[i] = -res[i];
        add_clause(root, res_neg);
        return;
    }else{
        for(int i=start; i<=end; i+=step){
            res[index] = i;
            dfs_cons2(root, i+step, end, step, k, index+1, res);
        }
    }
}

/**
 *
 * @param root
 * @param r1 one col/row
 * @param r2 another col/row
 * @param order
 * @param mode 1为行，2为列
 */
void tseytin(struct ClauseNode *root, int r1, int r2, int order, int mode)
{
    int *var_10;
    var_10 = (int *)malloc(sizeof(int) * (order+1));
    var_10[0] = order;
    for(int i=1;i<=order;i++){
        int var1, var2, new_var_1;
        if (mode == 1){
            var1 = (r1-1)*order+i;
            var2 = (r2-1)*order+i;
            new_var_1 = cur_add_num++;
        }else{
            var1 = r1+(i-1)*order;
            var2 = r2+(i-1)*order;
            new_var_1 = cur_add_num++;
        }
        int var_1[3] = {2, var1, -new_var_1};
        int var_2[3] = {2, var2, -new_var_1};
        int var_3[4] = {3, -var1, -var2, new_var_1};
        add_clause(root, var_1);add_clause(root, var_2);add_clause(root, var_3);
        int new_var_2 = cur_add_num++;
        int var_4[3] = {2, -var1, -new_var_2};
        int var_5[3] = {2, -var2, -new_var_2};
        int var_6[4] = {3, var1, var2, new_var_2};
        add_clause(root, var_4);add_clause(root, var_5);add_clause(root, var_6);
        int new_var_3 = cur_add_num++;
        int var_7[3] = {2, -new_var_1, new_var_3};
        int var_8[3] = {2, -new_var_2, new_var_3};
        int var_9[4] = {3, new_var_1, new_var_2, -new_var_3};
        add_clause(root, var_7);add_clause(root, var_8);add_clause(root, var_9);

        var_10[i] = -new_var_3;
    }
    add_clause(root, var_10);
}

void sudoku(struct ClauseNode *root)
{
    int order;
    int mode;
    int **board;
    mode = sudoku_load_display();
    printf("please enter the sudoku order:");
    scanf("%d", &order);

    board = board_loader(order, mode);
    show_board(board, order);
    sudoku_parser(board, root, order);
    save_cnf_file(root, board, order);

    int *truth_table = (int *)malloc(sizeof(int) * (cur_add_num+1));
    memset(truth_table, 0, sizeof(int) * (cur_add_num+1));
    if(dpll(root, &truth_table, 1)){
        play(board, order, truth_table);
        sudoku_result(truth_table, board, order);
    }else {
        printf("sudoku is unsolvable!");
    }

}

int ** board_loader(int order, int mode)
{
    int** board;
    board = (int **)malloc(sizeof(int *) * (order+1));
    for(int i=0;i<=order+1;i++) {
        board[i] = (int *)malloc(sizeof(int) * (order + 1));
        for(int j=0;j<order+1;j++) board[i][j] = -1;
    }
    if (mode == 1){
        char file_root[100];
        printf("please enter the file root:");
        scanf("%s", file_root);
        //parse the file
        FILE *fp;
        fp  = fopen(file_root, "r");
        if (fp==NULL){
            printf("open file failed!\n");
            exit(-1);
        }

        int cur_row = 1;
        while (!feof(fp)){
            char line[MAX_LINE_SIZE];
            fgets(line, MAX_LINE_SIZE, fp);
            if (feof(fp)) break;

            for(int i=0;i<order;i++){
                if (line[i] == '.') board[cur_row][i+1] = -1;
                if (line[i] == '1') board[cur_row][i+1] = 1;
                if (line[i] == '0') board[cur_row][i+1] = 0;
            }
            cur_row++;
        }
        return board;
    }else{
        terminal_generator(board, order);
//        show_board(board, order);
        return board_generator(board, order);
    }
}

int **board_generator(int **raw_board, int order)
{
//    for(int i=1;i<=order;i++){
//        for(int j=1;j<=order;j++){
//            if (is_follow_restrict(raw_board, order) && is_unique(raw_board, order, i, j)){
//                raw_board[i][j] = -1;
//            }
//        }
//    }
    int n, op=0;
    printf("how hard do you want to be:\n1. easy   2. medium   3. hard\nplease enter your choice:");
    scanf("%d", &op);
    switch (op) {
        case 1: n = (int)(order*order*0.1); break;
        case 2: n = (int)(order*order*0.3);break;
        case 3: n = (int)(order*order*0.5);break;
        default: n = (int)(order*order*0.1);
    }
    int i =0;
    srand((unsigned int)time(NULL));
    while (i<n){
        int r = rand()%order+1;
        int c = rand()%order+1;
        if (raw_board[r][c] == -1) continue;
        if (is_follow_restrict(raw_board, order) && is_unique(raw_board, order, r, c)){
            raw_board[r][c] = -1;i++;
        }
    }
    return raw_board;
}

status is_follow_restrict(int **raw_board, int order)
{
    //the remained cells are more than the lower bound
    int count=0;
    for(int i=1;i<=order;i++){
        for(int j=1;j<=order;j++){
            if (raw_board[i][j] != -1) count++;
        }
    }
    if (count < order*order*LOWER_BOUND_PROB) return FALSE;
    return TURE;
}

status is_unique(int **raw_board, int order, int r, int c)
{
    struct ClauseNode *root;
    root = (struct ClauseNode *)malloc(sizeof(struct ClauseNode));
    root->next = NULL;

    int temp = raw_board[r][c];
    raw_board[r][c] = raw_board[r][c] == 1 ? 0 : 1;
    sudoku_parser(raw_board, root, order);
    int *truth_table = (int *)malloc(sizeof(int) * (cur_add_num+1));
    memset(truth_table, 0, sizeof(int) * (cur_add_num+1));
//    save_cnf_file(root, raw_board, order);
//    show_data(root);
    if(dpll(root, &truth_table, 0)){
        raw_board[r][c] = temp;
        return FALSE;
    }
    else return TURE;
}

void terminal_generator(int **board, int order)
{
    struct ClauseNode *root;
    root = (struct ClauseNode*)malloc(sizeof(struct ClauseNode));
    root->next = NULL;
    srand((unsigned int)time(NULL));
    for(int i=0;i<GEN_NUM_PROB*order*order;){
        int r = rand()%order+1;
        int c = rand()%order+1;
        if (board[r][c] != -1) continue;
        board[r][c] = rand()%10>=5 ? 1 : 0;
        if (is_board_legal(board, order)) i++;
        else board[r][c] = -1;
    }

//    show_board(board,order);

    sudoku_parser(board, root, order);
    int *truth_table = (int *)malloc(sizeof(int) * (cur_add_num+1));
//    int truth_table[MAX_TRUTH_NUM];
//    for(int i=0;i<cur_add_num+1;i++) truth_table[i] = -1;
//    show_data(root);
    memset(truth_table, 0, sizeof(int) * (cur_add_num+1));
//    save_cnf_file(root, board, order);
    if (dpll(root, &truth_table, 0)){
        for(int i=1;i<=order*order;i++){
            if (truth_table[i]!=-1){
                int c = i%order;
                if (c == 0) c = order;
                int r = (i-c)/order+1;
                board[r][c] = truth_table[i];
            }
        }
    }else{
        printf("unsolvable!");
        exit(-1);
    }
}

status is_board_legal(int **board, int order)
{
    //cons1
    for(int i=1;i<=order;i++){
        for(int j=1;j<=order-2;j++){
            if (board[i][j] == -1) continue;
            if (board[i][j] == board[i][j+1] && board[i][j] == board[i][j+2]) return FALSE;
            if (board[j][i] == board[j+1][i] && board[j][i] == board[j+2][i]) return FALSE;
        }
    }

    //cons2
    for(int i=1;i<=order;i++){
        int count_row = 0;
        int count_col = 0;
        for(int j=1;j<=order;j++){
            if (board[i][j] == 1) count_row++;
            if (board[j][i] == 1) count_col++;
            if (count_col > order/2 || count_row > order/2) return FALSE;
        }
    }

    //cons3
    int flag_row = 0, flag_col=0;
    for(int i=1;i<=order-1;i++){
        for(int j=i;j<=order;j++){
            flag_row = 0;
            for(int k=1;k<=order;k++){
                if (board[i][k] != board[j][k] || board[i][k] == -1 || board[j][k] == -1){
                    flag_row = 1;
                    break;
                }
            }
            if (flag_row == 0) return FALSE;
        }
    }
    for(int i=1;i<=order-1;i++){
        for(int j=1;j<=order;j++){
            flag_col = 0;
            for(int k=1;k<=order;k++){
                if (board[k][i] != board[k][j] || board[k][i] == -1 || board[k][j] == -1){
                    flag_col = 1;
                    break;
                }
            }
            if (flag_col == 0) return FALSE;
        }
    }

    return TURE;
}

/**
 * 实现基础游戏功能
 * @param board
 * @param order
 * @param truth_table
 */
void play(int **board, int order, int *truth_table)
{
    int op=1, ans = 0, flag =0;
    int **res = (int **)malloc(sizeof(int *) * (order+1));
    for(int i=0;i<=order;i++){
        res[i] = (int *)malloc(sizeof(int) * (order+1));
        memset(res[i], 0, sizeof(int) * (order+1));
    }
    while (op){
        show_board(board, order);
        printf("please input your result:\n");
        for(int i=1;i<=order;i++){
            for(int j=1;j<=order;j++){
                scanf("%1d", &ans);
                res[i][j] = ans;
            }
        }

        //match
        flag = match_res(board, res, order, truth_table);
//        show_board(res,order);
        if (flag == 0){
            printf("you are wrong!!!!!!!!!!\n");
            printf("do you want to continue playing?\n");
            printf("1. YES!!!!!!\t\t0. NO-_-\n");
            printf("please enter your choice:");
            scanf("%d", &op);
        }
        else{
            printf("THAT IS CORRECT!!!! YOU ARE FANTASTIC!!!!\n");
            op = 0;
        }
    }
}

/**
 * judge if the player is correct
 * @param board
 * @param res
 * @param truth_table
 * @param order
 * @return correct:1, incorrect:0
 */
status match_res(int **board, int **res, int order, int *truth_table)
{
    for(int i=1;i<=order;i++){
        for(int j = 1;j<=order;j++){
            if (board[i][j] == -1){
                if (res[i][j] != truth_table[(i-1)*order+j]) return FALSE;
            }
            else{
                if (res[i][j] != board[i][j]) return FALSE;
            }
        }
    }
    return TURE;
}