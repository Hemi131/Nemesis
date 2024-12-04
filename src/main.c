#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"

int main() {
    /* printf("Hello, World!\n"); */
    char *str = malloc(100 * sizeof(char));
    struct queue *q;
    struct rpn_item *item;

    strcpy(str, "4+18/(2(9-3))");
    q = parse_to_rpn(str);

    if (!q) {
        printf("Failed to parse to RPN\n");
        return 1;
    }

    printf("RPN: ");
    item = malloc(sizeof(*item));
    while (queue_dequeue(q, item)) {
        switch (item->type) {
            case 'd':
                printf("%f ", item->data.number);
                break;
            case 'v':
                /* printf("var_%zu ", item->data.variable); */
                break;
            case '1':
            case '2':
                printf("%c ", item->data.operator);
                break;
            default:
                printf("Unknown type\n");
                break;
        }

        free(item);
        item = malloc(sizeof(*item));
    }
    free(item);
    return 0;
}

