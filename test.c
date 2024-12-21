int mainGG(int argc, char *argv[]) {
    int error_code = 0;
    char *input_file;
    char *output_file;

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
    

/* TODO: TADY JE TED OFFIKO ZACATEK */

    error_code = args_parser(argc, argv, &input_file, &output_file);

    if (error_code == EXIT_INVALID_INPUT_FILE) {
        printf("Input file not found!\n");
        goto fail_args111;
    }
    else if (error_code == EXIT_INVALID_OUTPUT_FILE) {
        printf("Invalid output destination!\n");
        goto fail_args111;
    }

/* FILE INPUT HANDLING */

    file = fopen(input_file, "r");
    if (!file) {
        printf("Failed to open file\n");
        return 1;
    }

    while (fgets(buffer, MAX_CHARS, file)) {
        if (buffer[0] == '\\' || buffer[0] == '\n') {
            continue;
        }
    input_parsing_start:
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
            goto input_parsing_start;

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
            goto input_parsing_start;
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


    if (!prepare_expression(purpose, allowed_vars, allowed_vars_count, buffer)) {
        printf("Failed to prepare purpose expression\n");
        printf("Unknown var: %s\n", buffer);
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
        if (!prepare_expression(buffer, allowed_vars, allowed_vars_count, buffer)) {
            printf("Failed to prepare subject expression\n");
            printf("Unknown var: %s\n", buffer);
            return 1;
        }
        
        expr1 = rpn_evaluate(parse_to_rpn(buffer), allowed_vars_count);

        token = strtok(NULL, "|");
        strcpy(buffer, token);
        if (!prepare_expression(buffer, allowed_vars, allowed_vars_count, buffer)) {
            printf("Failed to prepare subject expression\n");
            printf("Unknown var: %s\n", buffer);
            return 1;
        }
        expr2 = rpn_evaluate(parse_to_rpn(buffer), allowed_vars_count);

        exprResult = sub_evaluation_expressions(expr1, expr2);

        subject_expr[i] = exprResult;
    }


    for (i = 0; i < bounds_count; ++i) {
        token = strtok(bounds[i], "|");
        if (!prepare_expression(token, allowed_vars, allowed_vars_count, buffer)) {
            printf("Failed to prepare bounds expression\n");
            printf("Unknown var: %s\n", buffer);
            return 1;
        }
        expr1 = rpn_evaluate(parse_to_rpn(bounds[i]), allowed_vars_count);

        token = strtok(NULL, "|");
        if (!prepare_expression(token, allowed_vars, allowed_vars_count, buffer)) {
            printf("Failed to prepare bounds expression\n");
            printf("Unknown var: %s\n", buffer);
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


fail_args111:
    return error_code;
}