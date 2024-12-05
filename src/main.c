#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"

int main() {
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

