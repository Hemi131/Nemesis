#include "parser.h"
#include <string.h>
#include <stdlib.h>

#define MAX_EXPRESSION_LENGTH 200

#include <stdio.h>

int strlencmp(const char *str1, const char *str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    if (len1 < len2) {
        return -1;
    } else if (len1 > len2) {
        return 1;
    } else {
        return 0;
    }
}

int remove_substr(char *str, const char *substr) {
    char *pos, *temp;
    size_t len = strlen(substr);

    temp = malloc(strlen(str) + 1);
    if (!temp) {
        return EXIT_FAILURE;
    }

    while ((pos = strstr(str, substr)) != NULL) {
        strncpy(temp, str, pos - str);
        temp[pos - str] = '\0';

        strcat(temp, pos + len);

        strcpy(str, temp);
    }

    free(temp);
    return EXIT_SUCCESS;
}

int check_brackets(const char *str) {
    struct stack *s;
    char c;
    size_t i;

    if (!str) {
        return 0;
    }

    /* TODO: velikost zásobníku ? */
    s = stack_alloc(strlen(str), sizeof(char));

    if (!s) {
        return 0;
    }

    for (i = 0; str[i] != '\0'; ++i) {
        switch (str[i]) {
            case '(': /* TODO: nekontroluju takovou tu matematickou konvenci s pořadím závorek {[()]} */
            case '{':
            case '[':
                stack_push(s, &str[i]);
                break;
            case ')':
                if (!stack_pop(s, &c) || c != '(') {
                    goto wrong_brackets;
                }
                break;
            case '}':
                if (!stack_pop(s, &c) || c != '{') {
                    goto wrong_brackets;
                }
                break;
            case ']':
                if (!stack_pop(s, &c) || c != '[') {
                    goto wrong_brackets;
                }
                break;
            default:
                break;
        }
    }

    if (stack_item_count(s) == 0) {
        stack_dealloc(&s);
        return 1;
    }

wrong_brackets:
    stack_dealloc(&s);
    return 0;
}

int change_brackets(char *str) {
    size_t i;
    char *new_str = malloc((MAX_EXPRESSION_LENGTH + 1) * sizeof(char));

    if (!new_str) {
        return 0;
    }

    for (i = 0; str[i] != '\0'; ++i) {
        switch (str[i]) {
            case '{':
            case '[':
                new_str[i] = '(';
                break;
            case '}':
            case ']':
                new_str[i] = ')';
                break;
            default:
                new_str[i] = str[i];
                break;
        }
    }
    new_str[i] = '\0';

    strcpy(str, new_str);

    free(new_str);

    return 1;
}

/* int amend_multiplication_stars(char *str, const struct vector *allowed_variables) {  TODO: vyřadit 
    size_t i, j, offset;
    char *new_str = malloc((MAX_EXPRESSION_LENGTH + 1) * sizeof(char));

    if (!new_str) {
        return 0;
    }

    offset = 0;
    for (i = 0; str[i] != '\0'; ++i) {
        new_str[i + offset] = str[i];

        if (i + 1 < MAX_EXPRESSION_LENGTH && str[i] >= '0' && str[i] <= '9' && str[i+1] < '0' && str[i+1] > '9') {
            if (str[i + 1] == '(') {
                offset++;
                new_str[i + offset] = '*';
            }
            else {
                for (j = 0; j < vector_count(allowed_variables); ++j) {
                    if (str[i + 1] == *(char *)vector_at(allowed_variables, j)) {


                        offset++;
                        new_str[i + offset] = '*';
                        break;
                    }
                }
            }
            
        }

        if (i + offset >= MAX_EXPRESSION_LENGTH) {
            free(new_str);
            return 0;
        }
    }

    new_str[i] = '\0';

    strcpy(str, new_str);

    free(new_str);

    return 1;
} */

struct rpn_item *rpn_item_create_number(mat_num_type number) {
    struct rpn_item *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->type = 'd';
    new->data.number = number;

    return new;
}

struct rpn_item *rpn_item_create_variable(size_t variable) {
    struct rpn_item *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->type = 'v';
    new->data.variable = variable;

    return new;
}

struct rpn_item *rpn_item_create_operator_first_level(char operator) {
    struct rpn_item *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->type = '1';
    new->data.operator = operator;

    return new;
}

struct rpn_item *rpn_item_create_operator_second_level(char operator) {
    struct rpn_item *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->type = '2';
    new->data.operator = operator;

    return new;
}

struct rpn_item *rpn_item_create_bracket(char bracket) {
    struct rpn_item *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->type = 'b';
    new->data.bracket = bracket;

    return new;
}

struct evaluation_expression *create_evaluation_expression_constant(const size_t var_count, const mat_num_type constant) {
    struct evaluation_expression *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->constant = constant;
    new->var_count = var_count;
    new->var_koeficients = malloc(var_count * sizeof(mat_num_type));

    return new;
}

struct evaluation_expression *create_evaluation_expression_variable(const size_t var_count, const size_t variable_index) {
    struct evaluation_expression *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->constant = 0.0;
    new->var_count = var_count;
    new->var_koeficients = malloc(var_count * sizeof(mat_num_type));
    new->var_koeficients[variable_index] = 1.0;

    return new;
}

void free_evaluation_expression(struct evaluation_expression **expr) {
    if (!expr || !*expr) {
        return;
    }

    free((*expr)->var_koeficients);
    free(*expr);
    *expr = NULL;
}

struct evaluation_expression *add_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2) {
    size_t i;
    struct evaluation_expression *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->constant = expr1->constant + expr2->constant;
    new->var_count = expr1->var_count;
    new->var_koeficients = malloc(new->var_count * sizeof(mat_num_type));

    for (i = 0; i < new->var_count; ++i) {
        new->var_koeficients[i] = expr1->var_koeficients[i] + expr2->var_koeficients[i];
    }

    return new;
    
}

struct evaluation_expression *multiply_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2) {
    size_t i;
    struct evaluation_expression *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->constant = expr1->constant * expr2->constant;
    new->var_count = expr1->var_count;
    new->var_koeficients = malloc(new->var_count * sizeof(mat_num_type));

    for (i = 0; i < new->var_count; ++i) {
        new->var_koeficients[i] = expr1->var_koeficients[i] * expr2->constant;
    }

    return new;
}

struct evaluation_expression *sub_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2) {
    size_t i;
    struct evaluation_expression *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->constant = expr1->constant - expr2->constant;
    new->var_count = expr1->var_count;
    new->var_koeficients = malloc(new->var_count * sizeof(mat_num_type));

    for (i = 0; i < new->var_count; ++i) {
        new->var_koeficients[i] = expr1->var_koeficients[i] - expr2->var_koeficients[i];
    }

    return new;
}

struct evaluation_expression *divide_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2) {
    size_t i;
    struct evaluation_expression *new = malloc(sizeof(*new));

    if (!new) {
        return NULL;
    }

    new->constant = expr1->constant / expr2->constant;
    new->var_count = expr1->var_count;
    new->var_koeficients = malloc(new->var_count * sizeof(mat_num_type));

    for (i = 0; i < new->var_count; ++i) {
        new->var_koeficients[i] = expr1->var_koeficients[i] / expr2->constant;
    }

    return new;
}

struct queue *parse_to_rpn(const char *str) {
    struct stack *s;
    struct queue *q;

    int i, j, value;
    int last_was_number = 0;

    double number;

    char *endptr;

    struct rpn_item *rpn_item;

    char *buffer = malloc((MAX_EXPRESSION_LENGTH + 1) * sizeof(char));

    if (!str) {
        return NULL;
    }

    s = stack_alloc(strlen(str), sizeof(struct rpn_item));

    if (!s) {
        return NULL;
    }
    q = queue_alloc(strlen(str), sizeof(struct rpn_item));

    if (!q) {
        stack_dealloc(&s);
        return NULL;
    }

    for (i = 0; str[i] != '\0'; ++i) {
        switch (str[i]) {
            case '(': /* TODO: nekontroluju takovou tu matematickou konvenci s pořadím závorek {[()]} */
                if (last_was_number) {
                    rpn_item = rpn_item_create_operator_second_level('*');
                    if (!rpn_item) {
                        goto malloc_failed;
                    }
                    stack_push(s, rpn_item);
                }

                rpn_item = rpn_item_create_bracket('(');
                if (!rpn_item) {
                    goto malloc_failed;
                }
                stack_push(s, rpn_item);

                last_was_number = 0;
                break;
            case ')':
                while (stack_pop(s, rpn_item) && rpn_item->type != 'b') {
                    queue_enqueue(q, rpn_item);
                }
                free(rpn_item);

                last_was_number = 0;
                break;
            case '+':
            case '-':
                while (stack_head(s, rpn_item) && rpn_item->type == '2') {
                    stack_pop(s, rpn_item);
                    queue_enqueue(q, rpn_item);
                }
                rpn_item = rpn_item_create_operator_first_level(str[i]);
                if (!rpn_item) {
                    goto malloc_failed;
                }
                stack_push(s, rpn_item);

                last_was_number = 0;
                break;
            case '*':
            case '/':
                rpn_item = rpn_item_create_operator_second_level(str[i]);
                if (!rpn_item) {
                    goto malloc_failed;
                }
                stack_push(s, rpn_item);

                last_was_number = 0;
                break;
            case '@':

                if (last_was_number) {
                    rpn_item = rpn_item_create_operator_second_level('*');
                    if (!rpn_item) {
                        goto malloc_failed;
                    }
                    stack_push(s, rpn_item);
                }

                for (j = 1; str[i + j] != '@'; ++j) {
                    buffer[j - 1] = str[i + j];
                }
                buffer[j - 1] = '\0';

                value = strtol(buffer, &endptr, 10);
                if (*endptr != '\0') {
                    goto wrong_number;
                }

                rpn_item = rpn_item_create_variable(value);

                if (!rpn_item) {
                    goto malloc_failed;
                }

                if (!queue_enqueue(q, rpn_item)) {
                    goto queue_failed;
                }

                last_was_number = 0;
                i += j;
                break;
            default:
                for (j = 0; str[i + j] >= '0' && str[i + j] <= '9'; ++j) {
                    buffer[j] = str[i + j];
                }

                if (j == 0) {
                    goto bad_character;
                }

                buffer[j] = '\0';

                number = strtod(buffer, &endptr);

                if (*endptr != '\0') {
                    goto wrong_number;
                }

                rpn_item = rpn_item_create_number(number);

                if (!rpn_item) {
                    goto malloc_failed;
                }

                if (!queue_enqueue(q, rpn_item)) {
                    goto queue_failed;
                }

                last_was_number = 1;
                i += j - 1;
                break; 
        }
    }

    while (stack_pop(s, rpn_item)) {
        queue_enqueue(q, rpn_item);
    }

    goto end;

malloc_failed:
    printf("malloc failed\n");
wrong_number:
    printf("wrong number\n");
queue_failed:
    printf("queue failed\n");
bad_character:
    printf("bad character\n");
end:
    free(buffer);
    stack_dealloc(&s);

    return q;
}

/* struct evaluation_expression *rpn_evaluate(const struct queue *rpn) {
     TODO: 
    return NULL;
} */