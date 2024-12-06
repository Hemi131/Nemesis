#include "parser.h"
#include <string.h>
#include <stdlib.h>

#define MAX_EXPRESSION_LENGTH 200

#include <stdio.h>

int remove_substr(char *str, const char *substr) {
    char *pos, *temp;
    size_t len = strlen(substr);

    temp = malloc(strlen(str) + 1);
    if (!temp) {
        return 0;
    }

    while ((pos = strstr(str, substr)) != NULL) {
        strncpy(temp, str, pos - str);
        temp[pos - str] = '\0';

        strcat(temp, pos + len);

        strcpy(str, temp);
    }

    free(temp);
    return 1;
}

int check_valid_chars(const char *str) {
    size_t i;

    for (i = 0; str[i] != '\0'; ++i) {
        if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == '(' || str[i] == ')' || str[i] == '{' || str[i] == '}')) {
            return 0;
        }
    }

    return 1;
}

int replace_substr(char *str, const char *substr, const char *replacement) {
    char *pos, *temp;
    size_t len = strlen(substr);

    temp = malloc(strlen(str) + 1);
    if (!temp) {
        return EXIT_FAILURE;
    }

    while ((pos = strstr(str, substr)) != NULL) {
        strncpy(temp, str, pos - str);
        temp[pos - str] = '\0';

        strcat(temp, replacement);
        strcat(temp, pos + len);

        strcpy(str, temp);
    }

    free(temp);
    return EXIT_SUCCESS;
}

int replace_substr_with_end(char *str, const char *substr) {
    char *pos;

    pos = strstr(str, substr);
    if (pos != NULL) {
        pos[0] = '\0';
    }

    return EXIT_SUCCESS;
}

void sort_str_by_len(char *arr[], size_t n) {
    size_t i, j;

    for (i = 0; i < n - 1; ++i) {
        for (j = 0; j < n - i - 1; ++j) {
            if (strlen(arr[j]) < strlen(arr[j + 1])) {
                char *temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

void replace_vars_by_index(char *str, const char **vars, const size_t vars_count) {
    size_t i, j;
    char buffer[25];
    char **vars_sorted;

    vars_sorted = malloc(vars_count * sizeof(char *));
    if (!vars_sorted) {
        return;
    }
    memcpy(vars_sorted, vars, vars_count * sizeof(char *));
    sort_str_by_len(vars_sorted, vars_count);

    for (i = 0; i < vars_count; ++i) {
        for (j = 0; j < vars_count; ++j) {
            if (strcmp(vars[j], vars_sorted[i]) == 0) {
                break;
            }
        }

        sprintf(buffer, "{%lu}", j);
        replace_substr(str, vars_sorted[i], buffer);
    }

    free(vars_sorted);
}

int prepare_expression(char *str, const char **vars, const size_t vars_count) {
    if (!check_brackets(str)) {
        return 0;
    }

    if(!change_brackets(str)) {
        return 0;
    }

    replace_vars_by_index(str, vars, vars_count);

    if (!remove_substr(str, " ")) {
        return 0;
    }

    if (!check_valid_chars(str)) {
        return 0;
    }

    return 1;
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

struct rpn_item rpn_item_create_number(mat_num_type number) {
    struct rpn_item rpn_item;

    rpn_item.type = 'd';
    rpn_item.data.number = number;

    return rpn_item;
}

struct rpn_item rpn_item_create_variable(size_t variable) {
    struct rpn_item rpn_item;

    rpn_item.type = 'v';
    rpn_item.data.variable = variable;

    return rpn_item;
}

struct rpn_item rpn_item_create_operator_first_level(char operator) {
    struct rpn_item rpn_item;

    rpn_item.type = '1';
    rpn_item.data.operator = operator;

    return rpn_item;
}

struct rpn_item rpn_item_create_operator_second_level(char operator) {
    struct rpn_item rpn_item;

    rpn_item.type = '2';
    rpn_item.data.operator = operator;

    return rpn_item;
}

struct rpn_item rpn_item_create_bracket(char bracket) {
    struct rpn_item rpn_item;

    rpn_item.type = 'b';
    rpn_item.data.bracket = bracket;

    return rpn_item;
}

struct evaluation_expression create_evaluation_expression_constant(const mat_num_type constant) {
    struct evaluation_expression new_expr = {0};

    new_expr.constant = constant;
    new_expr.var_count = 0;

    return new_expr;
}

struct evaluation_expression create_evaluation_expression_variable(const size_t var_count, const size_t variable_index) {
    struct evaluation_expression new_expr = {0};

    new_expr.constant = 0.0;
    new_expr.var_count = var_count;
    new_expr.var_koeficients[variable_index] = 1.0;

    return new_expr;
}

struct evaluation_expression add_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2) {
    size_t i;
    struct evaluation_expression new_expr = {0};

    new_expr.constant = expr1.constant + expr2.constant;

    if (expr1.var_count || expr2.var_count) {
        new_expr.var_count = expr1.var_count > expr2.var_count ? expr1.var_count : expr2.var_count;
        for (i = 0; i < new_expr.var_count; ++i) {
            new_expr.var_koeficients[i] = expr1.var_koeficients[i] + expr2.var_koeficients[i];
        }
    }

    return new_expr;
    
}

struct evaluation_expression multiply_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2) {
    size_t i;
    struct evaluation_expression new_expr = {0};

    new_expr.constant = expr1.constant * expr2.constant;

    if (!expr1.var_count && !expr2.var_count) {
        return new_expr;
    }
    else if (expr1.var_count && !expr2.var_count) {
        new_expr.var_count = expr1.var_count > expr2.var_count ? expr1.var_count : expr2.var_count;
        for (i = 0; i < new_expr.var_count; ++i) {
            new_expr.var_koeficients[i] = expr1.var_koeficients[i] * expr2.constant;
        }
    }
    else if (!expr1.var_count && expr2.var_count) {
        new_expr.var_count = expr1.var_count > expr2.var_count ? expr1.var_count : expr2.var_count;
        for (i = 0; i < new_expr.var_count; ++i) {
            new_expr.var_koeficients[i] = expr2.var_koeficients[i] * expr1.constant;
        }
    }
    else
    {
        printf("Error: multiplication of two variables is not supported\n"); /* #TODO: error handling */
    }
    
    return new_expr;
}

struct evaluation_expression sub_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2) {
    size_t i;
    struct evaluation_expression new_expr = {0};

    new_expr.constant = expr1.constant - expr2.constant;

    if (expr1.var_count || expr2.var_count) {
        new_expr.var_count = expr1.var_count > expr2.var_count ? expr1.var_count : expr2.var_count;
        for (i = 0; i < new_expr.var_count; ++i) {
            new_expr.var_koeficients[i] = expr1.var_koeficients[i] - expr2.var_koeficients[i];
        }
    }

    return new_expr;
}

struct evaluation_expression divide_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2) {
    size_t i;
    struct evaluation_expression new_expr = {0};

    if (expr2.var_count) {
        printf("Error: division by variable is not supported.\n");
        exit(EXIT_FAILURE); /* TODO: error handling */
        return new_expr;
    }

    if (expr2.constant == 0) {
        printf("Error: division by zero\n");
        exit(EXIT_FAILURE); /* TODO: error handling */
        return new_expr;
    }

    new_expr.constant = expr1.constant / expr2.constant;

    if (expr1.var_count) {
        new_expr.var_count = expr1.var_count;
        for (i = 0; i < new_expr.var_count; ++i) {
            new_expr.var_koeficients[i] = expr1.var_koeficients[i] / expr2.constant;
        }
    }
    
    return new_expr;
}

struct queue *parse_to_rpn(const char *str) {
    struct stack *s;
    struct queue *q;

    int i, j, var_index;

    double number;

    char *endptr;

    struct rpn_item rpn_item, last_rpn_item;

    char *buffer = malloc((MAX_EXPRESSION_LENGTH + 1) * sizeof(char));

    if (!str || !buffer) {
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

    if (strlen(str) > 1 && (str[0] == '+' || str[0] == '-')) {
        rpn_item = rpn_item_create_number(0.0);

        if (!queue_enqueue(q, &rpn_item)) {
            goto queue_failed;
        }
    }

    last_rpn_item.type = '0';
    for (i = 0; str[i] != '\0'; ++i) {
        switch (str[i]) {
            case '(':
                if (last_rpn_item.type == 'd' || (last_rpn_item.type == 'b' && last_rpn_item.data.bracket == ')')) {

                    rpn_item = rpn_item_create_operator_second_level('*');

                    if (!stack_push(s, &rpn_item)) {
                        goto stack_failed;
                    }
                }

                rpn_item = rpn_item_create_bracket('(');

                if (!stack_push(s, &rpn_item)) {
                    goto stack_failed;
                }

                last_rpn_item = rpn_item;
                break;
            case ')':
                while (stack_pop(s, &rpn_item) && rpn_item.type != 'b') {
                    if (!queue_enqueue(q, &rpn_item)) {
                        goto queue_failed;
                    }
                }

                last_rpn_item = rpn_item_create_bracket(')');
                break;
            case '+':
            case '-':
                while (stack_head(s, &rpn_item) && rpn_item.type == '2') {
                    if (!stack_pop(s, &rpn_item)) {
                        goto stack_failed;
                    }
                    if (!queue_enqueue(q, &rpn_item)) {
                        goto queue_failed;
                    }
                }

                rpn_item = rpn_item_create_operator_first_level(str[i]);

                if (!stack_push(s, &rpn_item)) {
                    goto stack_failed;
                }

                last_rpn_item = rpn_item;
                break;
            case '*':
            case '/':
                rpn_item = rpn_item_create_operator_second_level(str[i]);
        
                if (!stack_push(s, &rpn_item)) {
                    goto stack_failed;
                }

                last_rpn_item = rpn_item;
                break;
            case '{':
                if (last_rpn_item.type == 'd') {
                    rpn_item = rpn_item_create_operator_second_level('*');

                    if (!stack_push(s, &rpn_item)) {
                        goto stack_failed;
                    }
                }

                for (j = 1; str[i + j] != '}'; ++j) {
                    buffer[j - 1] = str[i + j];
                }
                buffer[j - 1] = '\0';

                var_index = strtol(buffer, &endptr, 10);
                if (*endptr != '\0') {
                    goto wrong_number;
                }

                rpn_item = rpn_item_create_variable(var_index);

                if (!queue_enqueue(q, &rpn_item)) {
                    goto queue_failed;
                }

                last_rpn_item = rpn_item;
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

                if (!queue_enqueue(q, &rpn_item)) {
                    goto queue_failed;
                }

                last_rpn_item = rpn_item;
                i += j - 1;
                break; 
        }
    }

    while (stack_pop(s, &rpn_item)) {
        if (!queue_enqueue(q, &rpn_item)) {
            goto queue_failed;
        }
    }

    goto end;

stack_failed:
    printf("stack failed\n");
    goto end;
wrong_number:
    printf("wrong number\n");
    goto end;
queue_failed:
    printf("queue failed\n");
    goto end;
bad_character:
    printf("bad character\n");
end:
    free(buffer);
    stack_dealloc(&s);

    return q;
}

struct evaluation_expression rpn_evaluate(struct queue *rpn, const size_t var_count) {
    struct  stack *s;

    struct rpn_item rpn_item;
    struct evaluation_expression expr1, expr2, result;

    if (!rpn) {
        printf("Error: invalid RPN\n"); /* #TODO: error handling */
        return create_evaluation_expression_constant(0.0);
    }
    
    s = stack_alloc(queue_item_count(rpn), sizeof(struct evaluation_expression));

    if (!s) {
        printf("Error: stack allocation failed\n"); /* #TODO: error handling */
        return create_evaluation_expression_constant(0.0);
    }

    while (queue_dequeue(rpn, &rpn_item)) {
        switch (rpn_item.type) {
            case 'd':
            case 'v':
                if (rpn_item.type == 'v') {
                    expr1 = create_evaluation_expression_variable(var_count, rpn_item.data.variable);
                }
                else {
                    expr1 = create_evaluation_expression_constant(rpn_item.data.number);
                }

                if (!stack_push(s, &expr1)) {
                    printf("Error: stack push failed\n"); /* #TODO: error handling */
                    return create_evaluation_expression_constant(0.0);
                }
                break;
            case '1':
            case '2':
                if (!stack_pop(s, &expr2) || !stack_pop(s, &expr1)) {
                    printf("Error: invalid RPN\n"); /* #TODO: error handling */
                    return create_evaluation_expression_constant(0.0);
                }

                if (rpn_item.data.operator == '+') {
                    result = add_evaluation_expressions(expr1, expr2);
                }
                else if (rpn_item.data.operator == '-') {
                    result = sub_evaluation_expressions(expr1, expr2);
                }
                else if (rpn_item.data.operator == '*') {
                    result = multiply_evaluation_expressions(expr1, expr2);
                }
                else if (rpn_item.data.operator == '/') {
                    result = divide_evaluation_expressions(expr1, expr2);
                }
                else {
                    printf("Error: invalid RPN\n"); /* #TODO: error handling  bylo tam neco jinyho nez + nebo - nebo * nebo / */
                    return create_evaluation_expression_constant(0.0);
                }

                if (!stack_push(s, &result)) {
                    printf("Error: stack push failed\n"); /* #TODO: error handling */
                    return create_evaluation_expression_constant(0.0);
                }
                break;
            default:
                printf("Error: spatny operator type\n"); /* #TODO: error handling */
                return create_evaluation_expression_constant(0.0);
        }


    }

    if (!stack_pop(s, &result) || stack_item_count(s) != 0) {
        printf("Error: invalid RPN\n"); /* #TODO: error handling */
        return create_evaluation_expression_constant(0.0);
    }

    stack_dealloc(&s); /* TODO: nezapomenout na stack dealloc pri chybe */

    return result;
}