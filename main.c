#include "head.h"


int main() {
    setbuf(stdout, NULL);   //注意之后要删掉
    int op;
    menu();
    scanf("%d", &op);
    clock_t start, end, start_opt, end_opt;
    while (op){
        struct ClauseNode *root;
        int mode;
        root = (struct ClauseNode *)malloc(sizeof(struct ClauseNode));
//        root->next = (struct ClauseNode *)malloc(sizeof(struct ClauseNode)); //带表头的链表
        root->next = NULL;
        int *truth_table, *truth_table_opt;
        cur_var_count = 0; dif_var_count = 0;
        int flag=0, flag_opt=0;
        switch (op) {
            case 1:
                cnf_parser(root);

                truth_table =(int *)malloc(sizeof(int) * (num_var+1));
                truth_table_opt =(int *)malloc(sizeof(int) * (num_var+1));
                memset(truth_table, 0, sizeof(int) * (num_var+1));
                memset(truth_table_opt, 0, sizeof(int) * (num_var + 1));

                int is_opt;
                printf("do you want to run the optimization?\n1. YES\t2. NO\nplease enter your choice:");
                scanf("%d", &is_opt);

                if (is_opt == 1){
                    mode = mode_select();  //select the optimized mode
                    if (mode>1){
                        printf("wrong input");
                        break;
                    }
                    start = clock();
                    flag = dpll(root, &truth_table, 0);
                    end = clock();
//                for(int i=0;i<num_var+1;i++) if (truth_table[i] == -1) truth_table[i] = 0;
                    sat_result(flag, truth_table);
//                if (val_res(truth_table, root)) printf("The result is true\n");
//                else printf("The result is false\n");
                    double run_time = (double) (end-start) /CLK_TCK ;
                    printf("run time: %fs\n", run_time);
                    result(flag, truth_table, (int)(run_time*1000));
                    if (mode != 0) {
                        printf("-----------optimization----------\n");
                        start_opt = clock();
                        flag_opt = dpll(root, &truth_table_opt, mode);
                        end_opt = clock();
                        double run_time_opt = (double)(end_opt - start_opt) / CLK_TCK ;
//                    for(int i=0;i<num_var+1;i++) if (truth_table_opt[i] == -1) truth_table_opt[i] = 0;
                        sat_result(flag_opt, truth_table_opt);
//                    if (val_res(truth_table_opt, root)) printf("The result is true\n");
//                    else printf("The result is false\n");
                        printf("run time: %fs\n", run_time_opt);
                        printf("optimized: %f%%\n", (run_time - run_time_opt) / run_time * 100);
                        result(flag_opt, truth_table_opt, (int)(run_time*1000));
                    }
                }else{
                    mode = mode_select();  //select the optimized mode
                    if (mode>1){
                        printf("wrong input\n");
                        break;
                    }
                    start = clock();
                    flag = dpll(root, &truth_table, mode);
                    end = clock();
//                for(int i=0;i<num_var+1;i++) if (truth_table[i] == -1) truth_table[i] = 0;
                    sat_result(flag, truth_table);
                if (val_res(truth_table, root)) printf("The result is true\n");
                else printf("The result is false\n");
                    double run_time = (double) (end-start) /CLK_TCK ;
                    printf("run time: %fs\n", run_time);
                    result(flag, truth_table, (int)(run_time*1000));
                }
                break;
            case 2:
                sudoku(root);

            default:break;
        }
        menu();
        scanf("%d",&op);
    }
    return 0;
}