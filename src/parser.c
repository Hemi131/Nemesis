#include "parser.h"
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

int args_parser(int argc, char *argv[], char **input_file, char **output_file) {
    int i;

    *input_file = NULL;
    *output_file = NULL;

    if (argc < 2 || !argv) {
        return EXIT_INVALID_INPUT_FILE;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                *output_file = argv[i + 1];
                i++;
            }
            else {
                return EXIT_INVALID_OUTPUT_FILE;
            }
        }
        else {
            *input_file = argv[i];
        }
    }

    if (*input_file == NULL) {
        return EXIT_INVALID_INPUT_FILE;
    }

    return EXIT_SUCCESS;
}

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

int can_be_var(const char *str) {
    size_t i;

    for (i = 0; str[i] != '\0'; ++i) {
        if ((str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == '(' || str[i] == ')' || str[i] == '{' || str[i] == '}' || str[i] == '.' || str[i] == ':' || str[i] == '<' || str[i] == '>' || str[i] == '=')) {
            return 0;
        }
    }

    return 1;
}

int check_valid_chars(const char *str, char *unknown_var) {
    size_t i;
    int error = 0;

    for (i = 0; str[i] != '\0'; ++i) {
        if (!((str[i] >= '0' && str[i] <= '9') || str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' || str[i] == '(' || str[i] == ')' || str[i] == '{' || str[i] == '}' || str[i] == '.')) {
            strcpy(unknown_var, str + i);
            error = 1;
            break;
        }
    }

    if (error) {
        for (i = 0; unknown_var[i] != '\0'; ++i) {
            if ((unknown_var[i] == '+' || unknown_var[i] == '-' || unknown_var[i] == '*' || unknown_var[i] == '/' || unknown_var[i] == '(' || unknown_var[i] == ')' || unknown_var[i] == '{' || unknown_var[i] == '}' || unknown_var[i] == '.')) {
                unknown_var[i] = '\0';
                break;
            }
        }
        return 0;
    }

    return 1;
}

int replace_substr(char *str, const char *substr, const char *replacement) {
    char *pos, *temp;
    size_t len = strlen(substr);

    temp = malloc(MAX_CHARS);
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

void replace_vars_by_index(char *str, char **vars, const size_t vars_count) {
    size_t i, j;
    char buffer[MAX_CHARS];
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

int prepare_expression(char *str, char **vars, const size_t vars_count, char* unknown_var) {
    if (!check_brackets(str)) {
        return EXIT_SYNTAX_ERROR;
    }

    change_brackets(str);

    replace_vars_by_index(str, vars, vars_count);

    if (!remove_substr(str, " ")) {
        return EXIT_SYNTAX_ERROR;
    }

    if (!remove_substr(str, "\n")) {
        return EXIT_SYNTAX_ERROR;
    }

    if (!check_valid_chars(str, unknown_var)) {
        if (can_be_var(unknown_var)) {
            return EXIT_UNKNOWN_VAR;
        }
        return EXIT_SYNTAX_ERROR;
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

    s = stack_alloc(strlen(str), sizeof(char));

    if (!s) {
        return 0;
    }

    for (i = 0; str[i] != '\0'; ++i) {
        switch (str[i]) {
            case '(':
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

void change_brackets(char *str) {
    size_t i;
    char *new_str = malloc((MAX_EXPRESSION_LENGTH + 1) * sizeof(char));

    if (!new_str) {
        return;
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

    s = stack_alloc(strlen(str) + 1, sizeof(struct rpn_item)); /* +1 for possible operator on the beggining*/

    if (!s) {
        return NULL;
    }
    q = queue_alloc(strlen(str) + 1, sizeof(struct rpn_item));

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
                if (last_rpn_item.type == 'b' && last_rpn_item.data.bracket == '(') {
                    rpn_item = rpn_item_create_number(0.0);

                    if (!queue_enqueue(q, &rpn_item)) {
                        goto queue_failed;
                    }
                }

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
                for (j = 0; (str[i + j] >= '0' && str[i + j] <= '9') || str[i + j] == '.'; ++j) {
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

 int rpn_evaluate(struct queue *rpn, const size_t var_count, struct evaluation_expression* exprResult) {
    int error_code = EXIT_SUCCESS;
    struct  stack *s;

    struct rpn_item rpn_item;
    struct evaluation_expression expr1, expr2, result;

    if (!rpn) {
        error_code = EXIT_FAILURE;
        goto rpn_evaluate_failed;
    }
    
    s = stack_alloc(queue_item_count(rpn), sizeof(struct evaluation_expression));

    if (!s) {
        error_code = EXIT_FAILURE;
        goto rpn_evaluate_failed;
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
                    error_code = EXIT_FAILURE;
                    goto rpn_evaluate_failed;
                }
                break;
            case '1':
            case '2':
                if (!stack_pop(s, &expr2) || !stack_pop(s, &expr1)) {
                    error_code = EXIT_FAILURE;
                    goto rpn_evaluate_failed;
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
                    error_code = EXIT_FAILURE;
                    goto rpn_evaluate_failed;
                }

                if (!stack_push(s, &result)) {
                    error_code = EXIT_FAILURE;
                    goto rpn_evaluate_failed;
                }
                break;
            default:
                error_code = EXIT_FAILURE;
                goto rpn_evaluate_failed;
        }


    }

    if (!stack_pop(s, &result) || stack_item_count(s) != 0) {
        error_code = EXIT_FAILURE;
        goto rpn_evaluate_failed;
    }


    *exprResult = result;
rpn_evaluate_failed:
    stack_dealloc(&s);
    queue_dealloc(&rpn);

    return error_code;
}

struct problem_data *problem_data_alloc(const size_t var_count, const size_t subjects_count, const size_t bounds_count, const int problem_type) {
    struct problem_data *data = malloc(sizeof(struct problem_data));

    if (!data) {
        return NULL;
    }

    if (!problem_data_init(data, var_count, subjects_count, bounds_count, problem_type)) {
        free(data);
        return NULL;
    }

    return data;
}

int problem_data_init(struct problem_data *data, const size_t var_count, const size_t subjects_count, const size_t bounds_count, const int problem_type) {
    size_t i, j;

    if (!data) {
        return 0;
    }

    data->subjects_count = subjects_count;
    data->bounds_count = bounds_count;
    data->allowed_vars_count = var_count;
    data->problem_type = problem_type;

    data->subjects_expr = NULL;
    data->bounds_expr = NULL;
    data->allowed_vars = NULL;
    data->bounds_op = NULL;
    data->subjects_op = NULL;
    data->result = NULL;

    data->subjects_expr = malloc(data->subjects_count * sizeof(struct evaluation_expression));
    data->bounds_expr = malloc(data->bounds_count * sizeof(struct evaluation_expression));
    data->allowed_vars = malloc(data->allowed_vars_count * sizeof(char *));
    data->bounds_op = malloc(data->bounds_count * sizeof(int));
    data->subjects_op = malloc(data->subjects_count * sizeof(int));
    data->result = malloc(data->allowed_vars_count * sizeof(mat_num_type));
    data->unused_vars = malloc(data->allowed_vars_count * sizeof(int));

    if (data->allowed_vars) {
        for (i = 0; i < data->allowed_vars_count; i++) {
            data->allowed_vars[i] = NULL;
        }

        for (i = 0; i < data->allowed_vars_count; i++) {
            data->allowed_vars[i] = malloc(MAX_CHARS * sizeof(char));
            if (!data->allowed_vars[i]) {
                for (j = 0; j < i; j++) {
                    if (data->allowed_vars[j]) {
                        free(data->allowed_vars[j]);
                    }
                }
                free(data->allowed_vars);
                data->allowed_vars = NULL;
                break;
            }
        }
    }
    
    if (!data->subjects_expr || !data->bounds_expr || !data->allowed_vars || !data->bounds_op || !data->subjects_op || !data->result || !data->unused_vars) {
        if (data->subjects_expr) {
            free(data->subjects_expr);
        }

        if (data->bounds_expr) {
            free(data->bounds_expr);
        }

        if (data->allowed_vars) {
            free(data->allowed_vars);
        }

        if (data->bounds_op) {
            free(data->bounds_op);
        }

        if (data->subjects_op) {
            free(data->subjects_op);
        }

        if (data->result) {
            free(data->result);
        }

        if (data->unused_vars) {
            free(data->unused_vars);
        }

        return 0;
    }

    for (i = 0; i < data->allowed_vars_count; i++) {
        data->result[i] = 0.0;
        data->unused_vars[i] = 0;
    }

    return 1;
}

void problem_data_deinit(struct problem_data *data) {
    size_t i;

    if (!data) {
        return;
    }

    if (data->subjects_expr) {
        free(data->subjects_expr);
    }

    if (data->bounds_expr) {
        free(data->bounds_expr);
    }

    if (data->allowed_vars) {
        for (i = 0; i < data->allowed_vars_count; i++) {
            if (data->allowed_vars[i]) {
                free(data->allowed_vars[i]);
            }
        }

        free(data->allowed_vars);
    }

    if (data->bounds_op) {
        free(data->bounds_op);
    }

    if (data->subjects_op) {
        free(data->subjects_op);
    }

    if (data->result) {
        free(data->result);
    }

    if (data->unused_vars) {
        free(data->unused_vars);
    }
}

void problem_data_dealloc(struct problem_data *data) {
    if (!data) {
        return;
    }

    problem_data_deinit(data);
    free(data);
}

int input_parser(char *input_file, struct problem_data **problem_data, char *unknown_var) {
    int error_code = 0;
    int testReturn;
    int buffer_loaded;
    FILE *file;
    struct problem_data *data;

    char *buffer = NULL;
    char *purpose = NULL;
    char *generals = NULL;
    char **subjects = NULL;
    char **bounds = NULL;

    size_t subjects_count = 0;
    size_t bounds_count = 0;
    size_t allowed_vars_count = 0;
    int found_maximize = 0;
    int found_minimize = 0;

    size_t i;
    char *token;
    struct queue *queue;
    struct evaluation_expression expr1, expr2, exprResult;


    file = fopen(input_file, "r");
    if (!file) {
        error_code = EXIT_INVALID_INPUT_FILE;
        goto input_parsing_fail_input_file;
    }

    buffer = malloc((MAX_CHARS + 1) * sizeof(char));
    if (!buffer) {
        error_code = EXIT_MALLOC_ERROR;
        goto input_parsing_fail_buffer_malloc;
    }

    purpose = malloc((MAX_CHARS + 1) * sizeof(char));
    if (!purpose) {
        error_code = EXIT_MALLOC_ERROR;
        goto input_parsing_fail_purpose_malloc;
    }

    generals = malloc((MAX_CHARS + 1) * sizeof(char));
    if (!generals) {
        error_code = EXIT_MALLOC_ERROR;
        goto input_parsing_fail_generals_malloc;
    }

    subjects = malloc(MAX_LINES * sizeof(char *));
    if (!subjects) {
        error_code = EXIT_MALLOC_ERROR;
        goto input_parsing_fail_subjects_malloc;
    }

    for (i = 0; i < MAX_LINES; i++) {
        subjects[i] = NULL;
    }
    for (i = 0; i < MAX_LINES; i++) {
        subjects[i] = malloc((MAX_CHARS + 1) * sizeof(char));
        if (!subjects[i]) {
            error_code = EXIT_MALLOC_ERROR;
            goto input_parsing_fail_subject_item_malloc;
        }
    }

    bounds = malloc(MAX_LINES * sizeof(char *));

    if (!bounds) {
        error_code = EXIT_MALLOC_ERROR;
        goto input_parsing_fail_bounds_malloc;
    }

    for (i = 0; i < MAX_LINES; i++) {
        bounds[i] = NULL;
    }

    for (i = 0; i < MAX_LINES; i++) {
        bounds[i] = malloc((MAX_CHARS + 1) * sizeof(char));
        if (!bounds[i]) {
            error_code = EXIT_MALLOC_ERROR;
            goto input_parsing_fail_bounds_item_malloc;
        }
    }

    /* LOADING FILE */

    buffer_loaded = 0;
    while (buffer_loaded || fgets(buffer, MAX_CHARS, file)) {
        buffer_loaded = 0;
        if (buffer[0] == '\\' || buffer[0] == '\n') {
            continue;
        }

        if (strstr(buffer, "End")) {
            break;
        }

        if (strstr(buffer, "Subject To")) {
            while (fgets(buffer, MAX_CHARS, file) && !strstr(buffer, "Maximize") && !strstr(buffer, "Minimize")&& !strstr(buffer, "End") && !strstr(buffer, "Generals")&& !strstr(buffer, "Bounds")) {
                if (buffer[0] == '\\' || buffer[0] == '\n') {
                    continue;
                }

                replace_substr_with_end(buffer, "\\");
                token = strchr(buffer, ':');
                if (!token) {
                    error_code = EXIT_SYNTAX_ERROR;
                    goto input_parsing_fail_unknown_line;
                }
                token++;
                strcpy(subjects[subjects_count++], token);
            }
            buffer_loaded = 1;
        }

        else if (strstr(buffer, "Maximize")) {
            found_maximize = 1;
            if (!fgets(buffer, MAX_CHARS, file)) {
                error_code = EXIT_SYNTAX_ERROR;
                goto input_parsing_fail_unknown_line;
            }
            replace_substr_with_end(buffer, "\\");
            strcpy(purpose, buffer);
        }

        else if (strstr(buffer, "Minimize")) {
            found_minimize = 1;
            if (!fgets(buffer, MAX_CHARS, file)) {
                error_code = EXIT_SYNTAX_ERROR;
                goto input_parsing_fail_unknown_line;
            }
            replace_substr_with_end(buffer, "\\");
            strcpy(purpose, buffer);
        }

        else if (strstr(buffer, "Generals")) {
            if (!fgets(buffer, MAX_CHARS, file)) {
                error_code = EXIT_SYNTAX_ERROR;
                goto input_parsing_fail_unknown_line;
            }
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
            buffer_loaded = 1;
        }
  
        else {
            /* TODO: odebrat tohle vypsani */
            printf("Unknown line: %s\n", buffer);
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_unknown_line;
        }

    }

    if ((found_maximize && found_minimize) || (!found_maximize && !found_minimize)) {
            /* TODO: odebrat tohle vypsani */
            printf("Error: Objective not found or multiple objectives found\n");
            return EXIT_SYNTAX_ERROR;
        }

    /* PARSING INPUT */

    strcpy(buffer, generals);
    token = strtok(buffer, " ");
    while (token != NULL) {
        allowed_vars_count++;
        token = strtok(NULL, " ");
    }

    data = problem_data_alloc(allowed_vars_count, subjects_count, bounds_count, found_maximize ? MAXIMIZE : MINIMIZE);
    *problem_data = data;
    if (!data) {
        error_code = EXIT_MALLOC_ERROR;
        goto input_parsing_fail_data_alloc;
    }

    i = 0;
    token = strtok(generals, " ");
    while (token != NULL) {
        strcpy(data->allowed_vars[i], token);
        remove_substr(data->allowed_vars[i], "\n");
        i++;
        token = strtok(NULL, " ");
    }

    testReturn = prepare_expression(purpose, data->allowed_vars, data->allowed_vars_count, unknown_var);
    if (testReturn == EXIT_UNKNOWN_VAR) {
        error_code = EXIT_UNKNOWN_VAR;
        goto input_parsing_fail_purpose_expr;
    }
    else if (testReturn == EXIT_SYNTAX_ERROR) {
        error_code = EXIT_SYNTAX_ERROR;
        goto input_parsing_fail_purpose_expr;
    }

    queue = parse_to_rpn(purpose);
    if (!queue) {
        error_code = EXIT_SYNTAX_ERROR;
        goto input_parsing_fail_purpose_expr;
    }

    testReturn = rpn_evaluate(queue, data->allowed_vars_count, &(data->purpose_expr));
    if (testReturn == EXIT_FAILURE) {
        error_code = EXIT_SYNTAX_ERROR;
        goto input_parsing_fail_purpose_expr;
    }



    for (i = 0; i < data->subjects_count; ++i) {
        if (strstr(subjects[i], "<=")) {
            data->subjects_op[i] = -1;
            replace_substr(subjects[i], "<=", "|");
        } else if (strstr(subjects[i], ">=")) {
            data->subjects_op[i] = 1;
            replace_substr(subjects[i], ">=", "|");
        } else if (strstr(subjects[i], "=")) {
            data->subjects_op[i] = 0;
            replace_substr(subjects[i], "=", "|");
        } else {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_subject_op_expr;
        }
    }

    for (i = 0; i < data->bounds_count; ++i) {
        if (strstr(bounds[i], "<=")) {
            data->bounds_op[i] = -1;
            replace_substr(bounds[i], "<=", "|");
        } else if (strstr(bounds[i], ">=")) {
            data->bounds_op[i] = 1;
            replace_substr(bounds[i], ">=", "|");
        } else if (strstr(bounds[i], "=")) {
            data->bounds_op[i] = 0;
            replace_substr(bounds[i], "=", "|");
        } else {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_bounds_op_expr;
        }
    }

    for (i = 0; i < data->subjects_count; i++) {
        token = strtok(subjects[i], "|");
        strcpy(buffer, token);
        
        testReturn = prepare_expression(buffer, data->allowed_vars, data->allowed_vars_count, unknown_var);
        if (testReturn == EXIT_UNKNOWN_VAR) {
            error_code = EXIT_UNKNOWN_VAR;
            goto input_parsing_fail_subject_expr;
        }
        else if (testReturn == EXIT_SYNTAX_ERROR) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_subject_expr;
        }

        queue = parse_to_rpn(buffer);
        if (!queue) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_subject_expr;
        }
        testReturn = rpn_evaluate(queue, data->allowed_vars_count, &expr1);
        if (testReturn == EXIT_FAILURE) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_subject_expr;
        }


        token = strtok(NULL, "|");
        strcpy(buffer, token);
        testReturn = prepare_expression(buffer, data->allowed_vars, data->allowed_vars_count, unknown_var);
        if (testReturn == EXIT_UNKNOWN_VAR) {
            error_code = EXIT_UNKNOWN_VAR;
            goto input_parsing_fail_subject_expr;
        }
        else if (testReturn == EXIT_SYNTAX_ERROR) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_subject_expr;
        }

        queue = parse_to_rpn(buffer);
        if (!queue) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_subject_expr;
        }
        testReturn = rpn_evaluate(queue, data->allowed_vars_count, &expr2);

        exprResult = sub_evaluation_expressions(expr1, expr2);

        data->subjects_expr[i] = exprResult;
    }

    for (i = 0; i < data->bounds_count; i++) {
        token = strtok(bounds[i], "|");
        strcpy(buffer, token);
        
        testReturn = prepare_expression(buffer, data->allowed_vars, data->allowed_vars_count, unknown_var);
        if (testReturn == EXIT_UNKNOWN_VAR) {
            error_code = EXIT_UNKNOWN_VAR;
            goto input_parsing_fail_bound_expr;
        }
        else if (testReturn == EXIT_SYNTAX_ERROR) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_bound_expr;
        }

        queue = parse_to_rpn(buffer);
        if (!queue) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_bound_expr;
        }
        testReturn = rpn_evaluate(queue, data->allowed_vars_count, &expr1);
        if (testReturn == EXIT_FAILURE) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_bound_expr;
        }


        token = strtok(NULL, "|");
        strcpy(buffer, token);
        testReturn = prepare_expression(buffer, data->allowed_vars, data->allowed_vars_count, unknown_var);
        if (testReturn == EXIT_UNKNOWN_VAR) {
            error_code = EXIT_UNKNOWN_VAR;
            goto input_parsing_fail_bound_expr;
        }
        else if (testReturn == EXIT_SYNTAX_ERROR) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_bound_expr;
        }

        queue = parse_to_rpn(buffer);
        if (!queue) {
            error_code = EXIT_SYNTAX_ERROR;
            goto input_parsing_fail_bound_expr;
        }
        testReturn = rpn_evaluate(queue, data->allowed_vars_count, &expr2);

        exprResult = sub_evaluation_expressions(expr1, expr2);

        data->bounds_expr[i] = exprResult;
    }

input_parsing_fail_bound_expr:
input_parsing_fail_subject_expr:
input_parsing_fail_bounds_op_expr:
input_parsing_fail_subject_op_expr:
input_parsing_fail_purpose_expr:
input_parsing_fail_data_alloc:
input_parsing_fail_unknown_line:
input_parsing_fail_bounds_item_malloc:
    for (i = 0; i < MAX_LINES; i++) {
            if (bounds[i]) {
                free(bounds[i]);
            }
    }
    free(bounds);
input_parsing_fail_bounds_malloc:
input_parsing_fail_subject_item_malloc:
    for (i = 0; i < MAX_LINES; i++) {
        if (subjects[i]) {
            free(subjects[i]);
        }
    }
    free(subjects);
input_parsing_fail_subjects_malloc:
    free(generals);
input_parsing_fail_generals_malloc:
    free(purpose);
input_parsing_fail_purpose_malloc:
    free(buffer);
input_parsing_fail_buffer_malloc:
    fclose(file);
input_parsing_fail_input_file:

    return error_code;
}