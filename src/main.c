#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"

int main1() {
    /* printf("Hello, World!\n"); */
    char *str = malloc(100 * sizeof(char));
    struct queue *q;
/*     struct rpn_item item; */
    struct evaluation_expression expr;
    size_t i;

    strcpy(str, "4+18/(9-3)");
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


    expr = rpn_evaluate(q, 2);

    printf("\nResult: %f\n", expr.constant);
    for (i = 0; i < expr.var_count; ++i) {
        printf("var_%lu: %f\n", i, expr.var_koeficients[i]);
    }

    queue_dealloc(&q);
    free(str);

    return 0;
}

void print_array(char *arr[], size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        printf("%s\n", arr[i]);
    }
}

int main2() {
    char str[100] = "3x_1+ 2 * x_11";
    const char *vars[] = {"x_1", "x_11"};
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
    int subject_count = 0;
    int bounds_count = 0;
    int found_maximize = 0;
    int found_minimize = 0;
    int i;

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
                strcpy(subject[subject_count++], buffer);
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

    return 0;
}