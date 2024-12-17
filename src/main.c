#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"
#include "matrix.h"
#include "simplex.h"

int main1() {
    /* printf("Hello, World!\n"); */
    char *str = malloc(100 * sizeof(char));
    struct queue *q;
/*     struct rpn_item item; */
    struct evaluation_expression expr;
    size_t i;

    strcpy(str, "4.1+18/(9-3)");
    q = parse_to_rpn(str);

    if (!q) {
        printf("Failed to parse to RPN\n");
        return 1;
    }

    /* printf("RPN: ");
    while (queue_dequeue(q, &item)) {  nemuzu delat oboji protoze to vyndavam z ty fronty more!!!
        switch (item.type) {
            case 'd':
                printf("%f ", item.data.number);
                break;
            case 'v':
                printf("var_%lu ", item.data.variable);
                break;
            case '1':
            case '2':
                printf("%c ", item.data.operator);
                break;
            default:
                printf("Unknown type\n");
                break;
        }
    } */


    expr = rpn_evaluate(q, 0);

    printf("\nResult: %f\n", expr.constant);
    for (i = 0; i < expr.var_count; ++i) {
        printf("var_%lu: %f\n", i, expr.var_koeficients[i]);
    }

    free(str);

    return 0;
}

void print_array(char *arr[], size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        printf("%s\n", arr[i]);
    }
}

void print_array_d(int arr[], size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        printf("%d\n", arr[i]);
    }
}

void print_array_f(mat_num_type arr[], size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        printf("%f\n", arr[i]);
    }
}

void print_array_lu(size_t arr[], size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        printf("%lu\n", arr[i]);
    }
}

int main2() {
    char str[100] = "3x_1+ 2 * x_11";
    char *vars[] = {"x_1", "x_11"};
    size_t vars_count = 2;

    if (!prepare_expression(str, vars, vars_count)) {
        printf("Failed to prepare expression\n");
    }
    else {
        printf("Prepared expression: \"%s\"\n", str);
    }

    return 0;
}

int main() {
    #define MAX_CHARS 256
    #define MAX_LINES 20

    FILE *file;
    char buffer[MAX_CHARS];
    char purpose[MAX_CHARS] = "";
    char subject[MAX_LINES][MAX_CHARS];
    char bounds[MAX_LINES][MAX_CHARS];
    char generals[MAX_CHARS] = "";
    size_t subject_count = 0;
    size_t bounds_count = 0;
    int found_maximize = 0;
    int found_minimize = 0;
    size_t i, j, cols_count;

    struct evaluation_expression expr1, expr2, exprResult, purpose_expr, subject_expr[MAX_LINES], bounds_expr[MAX_LINES];
    int subject_op[MAX_LINES];
    int bounds_op[MAX_LINES];
    mat_num_type object_to[3 * MAX_VAR_COUNT];
    mat_num_type problem_type_coeficient = 1.0;
    char **allowed_vars;
    size_t allowed_vars_count = 0;
    char * token;

    struct matrix *mat;
    const mat_num_type M = 1000.0;
    size_t base_vars[MAX_VAR_COUNT];
    size_t base_vars_index;

    mat_num_type result[MAX_VAR_COUNT];

    memset(object_to, 0, sizeof(object_to));
    memset(base_vars, 0, sizeof(base_vars));
    memset(result, 0, sizeof(result));
    

/* FILE INPUT HANDLING */

    file = fopen("../test.lp", "r");
    if (!file) {
        printf("Failed to open file\n");
        return 1;
    }

    while (fgets(buffer, MAX_CHARS, file)) {
        if (buffer[0] == '\\' || buffer[0] == '\n') {
            continue;
        }
    start:
        if (strstr(buffer, "End")) {
            break;
        }

        if (strstr(buffer, "Subject To")) {
            while (fgets(buffer, MAX_CHARS, file) && !strstr(buffer, "Maximize") && !strstr(buffer, "Minimize")&& !strstr(buffer, "End") && !strstr(buffer, "Generals")&& !strstr(buffer, "Bounds")) {
                if (buffer[0] == '\\' || buffer[0] == '\n') {
                    continue;
                }

                replace_substr_with_end(buffer, "\\");
                strtok(buffer, ":");
                strcpy(subject[subject_count++], strtok(NULL, ":"));
            }
            goto start;

        }

        else if (strstr(buffer, "Maximize")) {
            found_maximize = 1;
            fgets(buffer, MAX_CHARS, file);
            replace_substr_with_end(buffer, "\\");
            strcpy(purpose, buffer);
        }

        else if (strstr(buffer, "Minimize")) {
            found_minimize = 1;
            fgets(buffer, MAX_CHARS, file);
            replace_substr_with_end(buffer, "\\");
            strcpy(purpose, buffer);
        }

        else if (strstr(buffer, "Generals")) {
            fgets(buffer, MAX_CHARS, file);
            replace_substr_with_end(buffer, "\\");
            strcpy(generals, buffer);
        }

        else if (strstr(buffer, "Bounds")) {
            while (fgets(buffer, MAX_CHARS, file) && !strstr(buffer, "Maximize") && !strstr(buffer, "Minimize")&& !strstr(buffer, "End") && !strstr(buffer, "Generals")&& !strstr(buffer, "Subject To")) {
                if (buffer[0] == '\\' || buffer[0] == '\n') {
                    continue;
                }
                replace_substr_with_end(buffer, "\\");
                strcpy(bounds[bounds_count++], buffer);
            }
            goto start;
        }
  
        else {
            printf("Unknown line: %s\n", buffer);
        }

    }

    fclose(file);
    

    if (found_maximize) {
        printf("Objective: Maximize\n");
    } else if (found_minimize) {
        printf("Objective: Minimize\n");
    } else {
        printf("Objective: Not found\n");
    }
    printf("Purpose: %s\n", purpose);
    printf("Subject To:\n");
    for (i = 0; i < subject_count; i++) {
        printf("%s", subject[i]);
    }
    printf("Generals: %s\n", generals);
    printf("Bounds:\n");
    for (i = 0; i < bounds_count; i++) {
        printf("%s", bounds[i]);
    }

    if ((found_maximize && found_minimize) || (!found_maximize && !found_minimize)) {
        printf("Error: Objective not found or multiple objectives found\n");
        return 1;
    }

/* PARSING INPUT */

    /* parsing allowed vars */
    allowed_vars = malloc(MAX_VAR_COUNT * sizeof(char *));
    for (i = 0; i < MAX_VAR_COUNT; i++) {
        allowed_vars[i] = malloc(MAX_CHARS * sizeof(char));
    }

    allowed_vars_count = 0;
    token = strtok(generals, " ");
    while (token != NULL) {
        remove_substr(token, "\n");
        strcpy(allowed_vars[allowed_vars_count++], token);
        token = strtok(NULL, " ");
    }

    printf("Allowed vars:\n");
    for (i = 0; i < allowed_vars_count; i++) {
        printf("%s\n", allowed_vars[i]);
    }


    if (!prepare_expression(purpose, allowed_vars, allowed_vars_count)) {
        printf("Failed to prepare purpose expression\n");
        return 1;
    }


    purpose_expr = rpn_evaluate(parse_to_rpn(purpose), allowed_vars_count);

    /* printf("Purpose expression const: %f\n", purpose_expr.constant);
    for (i = 0; i < purpose_expr.var_count; i++) {
        printf("var_%lu: %f\n", i, purpose_expr.var_koeficients[i]);
    }     */

    for (i = 0; i < subject_count; ++i) {
        
        if (strstr(subject[i], "<=")) {
            subject_op[i] = -1;
            replace_substr(subject[i], "<=", "|");
        } else if (strstr(subject[i], ">=")) {
            subject_op[i] = 1;
            replace_substr(subject[i], ">=", "|");
        } else if (strstr(subject[i], "=")) {
            subject_op[i] = 0;
            replace_substr(subject[i], "=", "|");
        } else {
            printf("Error: unknown operator in subject\n"); 
            /* TODO: error handling */
        }
    }

    for (i = 0; i < bounds_count; ++i) {
        if (strstr(bounds[i], "<=")) {
            bounds_op[i] = -1;
            replace_substr(bounds[i], "<=", "|");
        } else if (strstr(bounds[i], ">=")) {
            bounds_op[i] = 1;
            replace_substr(bounds[i], ">=", "|");
        } else if (strstr(bounds[i], "=")) {
            bounds_op[i] = 0;
            replace_substr(bounds[i], "=", "|");
        } else {
            printf("Error: unknown operator in bounds\n");
        }
    }

    /* print_array_d(subject_op, subject_count);
    print_array_d(bounds_op, bounds_count); */

    for (i = 0; i < subject_count; i++) {
        token = strtok(subject[i], "|");
        strcpy(buffer, token);
        if (!prepare_expression(buffer, allowed_vars, allowed_vars_count)) {
            printf("Failed to prepare subject expression\n");
            return 1;
        }
        
        expr1 = rpn_evaluate(parse_to_rpn(buffer), allowed_vars_count);

        token = strtok(NULL, "|");
        strcpy(buffer, token);
        if (!prepare_expression(buffer, allowed_vars, allowed_vars_count)) {
            printf("Failed to prepare subject expression\n");
            return 1;
        }
        expr2 = rpn_evaluate(parse_to_rpn(buffer), allowed_vars_count);

        exprResult = sub_evaluation_expressions(expr1, expr2);

        subject_expr[i] = exprResult;
    }


    for (i = 0; i < bounds_count; ++i) {
        token = strtok(bounds[i], "|");
        if (!prepare_expression(token, allowed_vars, allowed_vars_count)) {
            printf("Failed to prepare bounds expression\n");
            return 1;
        }
        expr1 = rpn_evaluate(parse_to_rpn(bounds[i]), allowed_vars_count);

        token = strtok(NULL, "|");
        if (!prepare_expression(token, allowed_vars, allowed_vars_count)) {
            printf("Failed to prepare bounds expression\n");
            return 1;
        }
        expr2 = rpn_evaluate(parse_to_rpn(token), allowed_vars_count);

        exprResult = sub_evaluation_expressions(expr1, expr2);

        bounds_expr[i] = exprResult;
    }



/* MATRIX PREP */

    cols_count = allowed_vars_count + 1; /* +1 for constant */
    for (i = 0; i < subject_count; i++) {
        if (subject_op[i] == 1) {
            cols_count += 2;
        }
        else {
            cols_count += 1;
        }
    }

    mat = matrix_allocate(subject_count, cols_count, 0.0);
    for (i = 0; i < subject_count; ++i) {
        for (j = 0; j < allowed_vars_count; ++j) {
            matrix_set(mat, i, j, subject_expr[i].var_koeficients[j]);
        }
        matrix_set(mat, i, cols_count - 1, -1.0 * subject_expr[i].constant); /* kvůli přehození na druhou stranu */
    }

    if (found_minimize) {
        problem_type_coeficient = -1.0;
    }
    for (i = 0; i < purpose_expr.var_count; ++i) {
        object_to[i] = problem_type_coeficient * purpose_expr.var_koeficients[i];
    }

    base_vars_index = 0;
    j = allowed_vars_count;
    for (i = 0; i < subject_count; ++i) {
        if (subject_op[i] == 1) {
            matrix_set(mat, i, j, -1.0);
            object_to[j] = 0.0;
            j++;
            matrix_set(mat, i, j, 1.0);
            object_to[j] = -M;
            base_vars[base_vars_index++] = j;
            j++;
        }
        else if (subject_op[i] == -1) {
            matrix_set(mat, i, j, 1.0);
            object_to[j] = 0.0;
            base_vars[base_vars_index++] = j;
            j++;
        }
        else {
            matrix_set(mat, i, j, 1.0);
            object_to[j] = -M;
            base_vars[base_vars_index++] = j;
            j++;
        }
    }

    printf("Base vars:\n");
    print_array_lu(base_vars, subject_count);
    printf("Object to:\n");
    print_array_f(object_to, cols_count);

/* SIMPLEX USE */

    simplex_maximize(mat, base_vars, object_to, result, allowed_vars_count);

    matrix_print(mat);

    for (i = 0; i < allowed_vars_count; ++i) {
        printf("%s = %f\n", allowed_vars[i], result[i]);
    }

/* FREE */

    for (i = 0; i < MAX_VAR_COUNT; ++i) {
        free(allowed_vars[i]);
    }
    free(allowed_vars);
    matrix_free(&mat);
    return 0;
}

int mainTT() {
    struct matrix *mat;
    size_t basic_vars[] = {2, 3, 5};
    mat_num_type object_to[] = {6.0, 4.0, 0.0, 0.0, 0.0, -1000.0};
    mat_num_type result[] = {0.0, 0.0};
    size_t i, vars_count = 2;

    mat = matrix_allocate(3, 7, 0.0);

    matrix_set(mat, 0, 0, 2.0);
    matrix_set(mat, 0, 1, 3.0);
    matrix_set(mat, 0, 2, 1.0);
    matrix_set(mat, 0, 6, 30.0);
    matrix_set(mat, 1, 0, 3.0);
    matrix_set(mat, 1, 1, 2.0);
    matrix_set(mat, 1, 3, 1.0);
    matrix_set(mat, 1, 6, 24.0);
    matrix_set(mat, 2, 0, 1.0);
    matrix_set(mat, 2, 1, 1.0);
    matrix_set(mat, 2, 4, -1.0);
    matrix_set(mat, 2, 5, 1.0);
    matrix_set(mat, 2, 6, 3.0);

    simplex_maximize(mat, basic_vars, object_to, result, 2);

    matrix_print(mat);
    
    matrix_free(&mat);

    for (i = 0; i < vars_count; ++i) {
        printf("x_%lu: %f\n", i, result[i]);
    }

    return 0;
}