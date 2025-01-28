#include "simplex.h"

#ifdef SIMPLEX_DEBUG
#include <stdio.h>
#endif

#include <stdlib.h>

int _matrix_normalize_row_by_col(struct matrix *mat, size_t row, size_t col) {
    double divider;
    size_t i;

    if (!mat || !mat->items || row >= mat->rows || col >= mat->cols) {
            return 0;
        }

    divider = matrix_get(mat, row, col);

    if (divider == 0) {
        return 0;
    }

    for (i = 0; i < mat->cols; i++) {
        matrix_set(mat, row, i, matrix_get(mat, row, i) / divider);
    }

    return 1;
}

int _matrix_pivoting(struct matrix *mat, size_t row, size_t col) {
    double multiplier;
    size_t i, j;

    if (!mat || !mat->items || row >= mat->rows || col >= mat->cols) {
        return 0;
    }

    if (!_matrix_normalize_row_by_col(mat, row, col)) {
        return 0;
    }

    for (i = 0; i < mat->rows; i++) {
        if (i == row) {
            continue;
        }

        multiplier = matrix_get(mat, i, col);

        for (j = 0; j < mat->cols; j++) {
            matrix_set(mat, i, j, matrix_get(mat, i, j) - multiplier * matrix_get(mat, row, j));
        }
    }

    return 1;
}

int _smallest_quotient_row_index(const struct matrix *mat, size_t col, size_t *row_to_optimize) {
    size_t i, have_first_candidate;
    double min_value, test;

    if (!mat || !mat->items || col >= mat->cols || mat->rows == 0) {
        return -1;
    }

    have_first_candidate = 0;

    for (i = 0; i < mat->rows; i++) {
        test = matrix_get(mat, i, mat->cols - 1) / matrix_get(mat, i, col);

        if (test > 0) {
            if (!have_first_candidate) {
                min_value = test;
                *row_to_optimize = i;
                have_first_candidate = 1;
            }
            else if (test < min_value) {
                min_value = test;
                *row_to_optimize = i;
            }
        }
    }

    if (!have_first_candidate) {
        return 0;
    }
    else {
        return 1;
    }
}

int optimal_max_test(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, size_t *col_to_optimize) {
    mat_num_type max_value, test_value;
    size_t col_index, row_index;

    if (!mat || !mat->items || !base_vars || !object_to || !col_to_optimize) {
        return -1;
    }

    max_value = -THE_BIG_M * THE_BIG_M;
    for (col_index = 0; col_index < mat->cols - 1; col_index++) {
        test_value = object_to[col_index];

        for (row_index = 0; row_index < mat->rows; row_index++) {
            test_value -= matrix_get(mat, row_index, col_index) * object_to[base_vars[row_index]];
        }

        #ifdef SIMPLEX_DEBUG
        printf("Test value: %f\n", test_value);
        #endif

        if (test_value == 0.0) {
            continue;
        }

        if (test_value > max_value) {
            max_value = test_value;
            *col_to_optimize = col_index;
        }
    }

    if (max_value <= 0) {
        return 1;
    }
    else {
        return 0;
    }
}

int simplex_valid_base(struct matrix *mat, size_t *base_vars, size_t not_valid_basis_count, size_t *not_valid_basis) {
    size_t i, j;

    for (i = 0; i < mat->rows; i++) {
        for (j = 0; j < not_valid_basis_count; j++) {
            if (base_vars[i] == not_valid_basis[j]) {
                return 0;
            }
        }
    }

    return 1;
}

int simplex_maximize(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, mat_num_type *result, size_t real_vars_count, size_t *not_valid_basis, size_t not_valid_basis_count) {
    size_t smallest_quotient_row, smallest_quotient_col, row_index, i, iterations_count;
    
    if (!mat || !mat->items || !base_vars || !object_to) {
        return EXIT_MALLOC_ERROR;
    }

    iterations_count = 0;

    while ((!optimal_max_test(mat, base_vars, object_to, &smallest_quotient_col) || !simplex_valid_base(mat, base_vars, not_valid_basis_count, not_valid_basis)) && iterations_count++ < MAX_ITERATIONS_COUNT) {
        if (!_smallest_quotient_row_index(mat, smallest_quotient_col, &smallest_quotient_row)) {
            return EXIT_OBJECTIVE_UNBOUNDED;
        }

        /* Změna báze řádku simplexové tabulky. */
        base_vars[smallest_quotient_row] = smallest_quotient_col;

        #ifdef SIMPLEX_DEBUG
        printf("Pivot row: %lu, Pivot col: %lu\n", smallest_quotient_row, smallest_quotient_col);
        matrix_print(mat);
        #endif

        if (!_matrix_pivoting(mat, smallest_quotient_row, smallest_quotient_col)) {
            return EXIT_OBJECTIVE_INFEASIBLE;
        }

        /* Vynulování výsledků. */
        for (i = 0; i < real_vars_count; i++) {
            result[i] = 0.0;
        }

        /* Výpočet výsledků. */
        for (row_index = 0; row_index < mat->rows; row_index++) {
            /* Pokud není proměnná z původního problému, tak nepočitej její aktuální hodnotu */
            if (base_vars[row_index] >= real_vars_count) {
                continue;
            }
            result[base_vars[row_index]] = matrix_get(mat, row_index, mat->cols - 1);

            #ifdef SIMPLEX_DEBUG
            printf("result[%lu] = %f\n", base_vars[row_index], result[base_vars[row_index]]);
            #endif
        }
    }

    #ifdef SIMPLEX_DEBUG
    matrix_print(mat);
    #endif

    /* Pokud zůstala v bázi nevalidní proměnná, řešení není korektní. */
    if (!simplex_valid_base(mat, base_vars, not_valid_basis_count, not_valid_basis)) {
        return EXIT_OBJECTIVE_INFEASIBLE;
    }

    return EXIT_SUCCESS;
}

int simplex(struct problem_data *data) {
    int error_code = 0;
    const mat_num_type M = THE_BIG_M;
    size_t i, j, cols_count;
    struct matrix *mat;
    mat_num_type problem_type_coeficient = 1.0;
    mat_num_type *object_to;
    size_t *base_vars, base_vars_index, *not_valid_basis, not_valid_basis_count;

    /* Knotrola hodnot pravých stran Subject to. */
    for (i = 0; i < data->subjects_count; i++) {

        /* Pokud je pravá strana rovnice negativní, vynásobení rovnice -1 a otočení znaménka. */
        if ((data->subjects_expr[i]).constant > 0.0) {
            for (j = 0; j < data->allowed_vars_count; j++) {
                (data->subjects_expr[i]).var_coeficients[j]   *= -1.0;
            }
            (data->subjects_expr[i]).constant *= -1.0;

            data->subjects_op[i] *= -1;
        }
        /* Pokud je pravá strana rovnice rovna 0 a operátor rovnice je >=, vynásobení rovnice -1 a otočení znaménka. */
        else if ((data->subjects_expr[i]).constant == 0.0 && data->subjects_op[i] == 1) {
            for (j = 0; j < data->allowed_vars_count; j++) {
                (data->subjects_expr[i]).var_coeficients[j]   *= -1.0;
            }
            (data->subjects_expr[i]).constant *= -1.0;

            data->subjects_op[i] *= -1;
        }
    }

    /* Knotrola hodnot pravých stran Bounds. */
    for (i = 0; i < data->bounds_count; i++) {

        /* Pokud je pravá strana rovnice negativní, vynásobení rovnice -1 a otočení znaménka. */
        if ((data->bounds_expr[i]).constant > 0.0) {
            for (j = 0; j < data->allowed_vars_count; j++) {
                (data->bounds_expr[i]).var_coeficients[j]   *= -1.0;
            }
            (data->bounds_expr[i]).constant *= -1.0;

            data->bounds_op[i] *= -1;
        }
        /* Pokud je pravá strana rovnice rovna 0 a operátor rovnice je >=, vynásobení rovnice -1 a otočení znaménka. */
        else if ((data->bounds_expr[i]).constant == 0.0 && data->bounds_op[i] == 1) {
            for (j = 0; j < data->allowed_vars_count; j++) {
                (data->bounds_expr[i]).var_coeficients[j]   *= -1.0;
            }
            (data->bounds_expr[i]).constant *= -1.0;

            data->bounds_op[i] *= -1;
        }
    }

    /* Výpočet celkového počtu sloupců simplexové tabulky. */
    cols_count = data->allowed_vars_count + 1; /* +1 pro konstanty pravých stran */
    for (i = 0; i < data->subjects_count; i++) {
        if (data->subjects_op[i] == 1) {
            cols_count += 2;
        }
        else {
            cols_count += 1;
        }
    }
    for (i = 0; i < data->bounds_count; i++) {
        if (data->bounds_op[i] == 1) {
            cols_count += 2;
        }
        else {
            cols_count += 1;
        }
    }

    /* Alokace simplexové tabulky. */
    mat = matrix_allocate(data->subjects_count + data->bounds_count, cols_count, 0.0);
    if (!mat) {
        error_code = EXIT_MALLOC_ERROR;
        goto simplex_matrix_malloc_fail;
    }

    /* Alokace pole koeficientů proměnných účelové funkce.  */
    object_to = malloc(cols_count * sizeof(mat_num_type));
    if (!object_to) {
        error_code = EXIT_MALLOC_ERROR;
        goto simplex_object_to_malloc_fail;
    }

    /* Alokace pole indexů bázových proměnných. */
    base_vars = malloc((data->subjects_count + data->bounds_count) * sizeof(size_t));
    if (!base_vars) {
        error_code = EXIT_MALLOC_ERROR;
        goto simplex_base_vars_malloc_fail;
    }

    /* Alokace pole indexů nevalidních bázových proměnných. */
    not_valid_basis = malloc(cols_count * sizeof(size_t));
    if (!not_valid_basis) {
        error_code = EXIT_MALLOC_ERROR;
        goto simplex_not_valid_basis_malloc_fail;
    }

    /* Naplnění simplexové tabulky daty Subject to. */
    for (i = 0; i < data->subjects_count; i++) {
        for (j = 0; j < data->allowed_vars_count; j++) {
            matrix_set(mat, i, j, data->subjects_expr[i].var_coeficients[j]);
        }
        matrix_set(mat, i, cols_count - 1, -1.0 * data->subjects_expr[i].constant); /* kvůli přehození na druhou stranu */
    }

    /* Naplnění simplexové tabulky daty Bounds. */
    for (i = 0; i < data->bounds_count; i++) {
        for (j = 0; j < data->allowed_vars_count; j++) {
            matrix_set(mat, i + data->subjects_count, j, data->bounds_expr[i].var_coeficients[j]);
        }
        matrix_set(mat, i + data->subjects_count, cols_count - 1, -1.0 * data->bounds_expr[i].constant);  /* kvůli přehození na druhou stranu  */
    }

    /* Identifikace typu problému. */
    if (data->problem_type == MINIMIZE) {
        problem_type_coeficient = -1.0;
    }

    /* Naplnění pole koeficientů původních proměnných účelové funkce. 
       Při minimalizaci jsou obráceny koeficienty původních proměnných účelové funkce. */
    for (i = 0; i < data->purpose_expr.var_count; i++) {
        object_to[i] = problem_type_coeficient * data->purpose_expr.var_coeficients[i];
    }

    /* Naplnění pole koeficientů pomocných proměnných účelové funkce a simplexové tabulky. */
    base_vars_index = 0;
    not_valid_basis_count = 0;
    j = data->allowed_vars_count;
    for (i = 0; i < data->subjects_count; i++) {
        if (data->subjects_op[i] == 1) {
            matrix_set(mat, i, j, -1.0);
            object_to[j] = 0.0;
            j++;
            matrix_set(mat, i, j, 1.0);
            object_to[j] = -M;
            base_vars[base_vars_index++] = j;
            not_valid_basis[not_valid_basis_count++] = j;
            j++;
        }
        else if (data->subjects_op[i] == -1) {
            matrix_set(mat, i, j, 1.0);
            object_to[j] = 0.0;
            base_vars[base_vars_index++] = j;
            j++;
        }
        else {
            matrix_set(mat, i, j, 1.0);
            object_to[j] = -M;
            base_vars[base_vars_index++] = j;
            not_valid_basis[not_valid_basis_count++] = j;
            j++;
        }
    }

    /* Naplnění pole koeficientů pomocných proměnných účelové funkce a simplexové tabulky. */
    for (i = 0; i < data->bounds_count; i++) {
        if (data->bounds_op[i] == 1) {
            matrix_set(mat, i + data->subjects_count, j, -1.0);
            object_to[j] = 0.0;
            not_valid_basis[not_valid_basis_count++] = j;
            j++;
            matrix_set(mat, i + data->subjects_count, j, 1.0);
            object_to[j] = -M;
            base_vars[base_vars_index++] = j;
            not_valid_basis[not_valid_basis_count++] = j;
            j++;
        }
        else if (data->bounds_op[i] == -1) {
            matrix_set(mat, i + data->subjects_count, j, 1.0);
            object_to[j] = 0.0;
            base_vars[base_vars_index++] = j;
            j++;
        }
        else {
            matrix_set(mat, i + data->subjects_count, j, 1.0);
            object_to[j] = -M;
            base_vars[base_vars_index++] = j;
            not_valid_basis[not_valid_basis_count++] = j;
            j++;
        }
    }

    /* Detekce nepoužitých proměnných. */
    for (i = 0; i < data->allowed_vars_count; i++) {
        if (is_column_zero(mat, i)) {
            data->unused_vars[i] = UNUSED_VAR;
        }
    }

    /* Spuštění simplexové metody. */
    error_code = simplex_maximize(mat, base_vars, object_to, data->result, data->allowed_vars_count, not_valid_basis, not_valid_basis_count);
    
/* Uvolnění alokovaných prostředků. */

    free(not_valid_basis);
simplex_not_valid_basis_malloc_fail:
    free(base_vars);
simplex_base_vars_malloc_fail:
    free(object_to);
simplex_object_to_malloc_fail:
    matrix_free(&mat);
simplex_matrix_malloc_fail:
    return error_code;
}