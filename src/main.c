#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"
#include "matrix.h"
#include "simplex.h"

/**
 * \brief Funkce vytiskne výsledky na výstup specifikovaný v *outputfile, pokud NULL, vytiskne se na stdout.
 * \param `*output_file` Ukazatel na řetězec s cestou k souboru.
 * \param `*data` Ukazatel na data LP úlohy.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak kód chyby.
 */
int printResults(char *output_file, struct problem_data *data) {
    FILE *file;
    size_t i;

    if (output_file) {
        file = fopen(output_file, "w");
    }
    else {
        file = stdout;
    }

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

    if (output_file) {
        fclose(file);
    }
    return EXIT_SUCCESS;
}

/**
 * \brief Vstupní bod programu.
 * \param `argc` Počet argumentů.
 * \param `*argv[]` Pole argumentů.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak kód chyby.
 */
int main(int argc, char *argv[]) {
    int error_code = 0;
    char *input_file;
    char *output_file;

    struct problem_data *data = NULL;

    char unknown_var[MAX_CHARS];

    size_t i;

/* ARGS PARSING */

    error_code = args_parser(argc, argv, &input_file, &output_file);

    if (error_code == EXIT_INVALID_INPUT_FILE) {
        printf("Input file not found!\n");
        goto main_free;
    }
    else if (error_code == EXIT_INVALID_OUTPUT_FILE) {
        printf("Invalid output destination!\n");
        goto main_free;
    }

/* INPUT PARSING */

    error_code = input_parser(input_file, &data, unknown_var);

    if (error_code == EXIT_INVALID_INPUT_FILE) {
        printf("Input file not found!\n");
        goto main_free;
    }
    else if (error_code == EXIT_SYNTAX_ERROR) {
        printf("Syntax error!\n");
        goto main_free;
    }
    else if (error_code == EXIT_MALLOC_ERROR) {
        printf("Memory allocation error!\n");
        goto main_free;
    }
    else if (error_code == EXIT_UNKNOWN_VAR) {
        printf("Unknown variable '%s'!\n", unknown_var);
        goto main_free;
    }

/* USE OF SIMPLEX */

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
        error_code = printResults(output_file, data);
        if (error_code == EXIT_INVALID_OUTPUT_FILE) {
            printf("Invalid output destination!\n");
        }
    }

/* FREE */

main_free:
    problem_data_dealloc(data);
    return error_code;
}