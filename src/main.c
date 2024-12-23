#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"
#include "matrix.h"
#include "simplex.h"

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

int printResultsToFile(char *output_file, struct problem_data *data) {
    FILE *file = fopen(output_file, "w");
    size_t i;

    if (!file) {
        return EXIT_INVALID_OUTPUT_FILE;
    }

    if (data->result) {
        for (i = 0; i < data->allowed_vars_count; ++i) {
            if (data->unused_vars[i] != UNUSED_VAR) {
                fprintf(file, "%s = %f\n", data->allowed_vars[i], data->result[i]);
            }
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    int error_code = 0;
    char *input_file;
    char *output_file;

    struct problem_data *data = NULL;

    char unknown_var[MAX_CHARS];

    size_t i;

    error_code = args_parser(argc, argv, &input_file, &output_file);

    if (error_code == EXIT_INVALID_INPUT_FILE) {
        printf("Input file not found!\n");
        goto fail_args;
    }
    else if (error_code == EXIT_INVALID_OUTPUT_FILE) {
        printf("Invalid output destination!\n");
        goto fail_args;
    }

    error_code = input_parser(input_file, &data, unknown_var);

    if (error_code == EXIT_INVALID_INPUT_FILE) {
        printf("Input file not found!\n");
        goto fail_args;
    }
    else if (error_code == EXIT_SYNTAX_ERROR) {
        printf("Syntax error!\n");
        goto fail_args;
    }
    else if (error_code == EXIT_MALLOC_ERROR) {
        printf("Memory allocation error!\n");
        goto fail_args;
    }
    else if (error_code == EXIT_UNKNOWN_VAR) {
        printf("Unknown variable '%s'!\n", unknown_var);
        goto fail_args;
    }

    error_code = simplex(data);

    for (i = 0; i < data->allowed_vars_count; ++i) {
        if (data->unused_vars[i] == UNUSED_VAR) {
            printf("Warning: unused variable '%s'!\n", data->allowed_vars[i]);
        }
    }

    if (error_code == EXIT_OBJECTIVE_UNBOUNDED) {
        printf("Objective function is unbounded.\n");
    }
    else if (error_code == EXIT_OBJECTIVE_INFEASIBLE) {
        printf("No feasible solution exists.\n");
    }
    else if (error_code == EXIT_SUCCESS) {
        if (output_file) {
            error_code = printResultsToFile(output_file, data);
            if (error_code == EXIT_INVALID_OUTPUT_FILE) {
                printf("Invalid output destination!\n");
                goto fail_args;
            }
        }
        else {
            for (i = 0; i < data->allowed_vars_count; ++i) {
                if (data->unused_vars[i] != UNUSED_VAR) {
                    printf("%s = %f\n", data->allowed_vars[i], data->result[i]);
                }
            }
        }
    }

fail_args:
    problem_data_dealloc(data);
    return error_code;
}