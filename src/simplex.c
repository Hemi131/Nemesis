/* https://math.libretexts.org/Bookshelves/Applied_Mathematics/Applied_Finite_Mathematics_(Sekhon_and_Bloom)/04%3A_Linear_Programming_The_Simplex_Method/4.02%3A_Maximization_By_The_Simplex_Method */
#include "simplex.h"

#include <stdio.h>
#include <stdlib.h>

void _matrix_normalize_row_by_col(struct matrix *mat, size_t row, size_t col) {
    double divider;
    size_t i;

    if (!mat || !mat->items || row >= mat->rows || col >= mat->cols) {
            return;
        }

    divider = matrix_get(mat, row, col);

    if (divider == 0) {
        printf("Division by zero.\n"); /* #TODO: mozna nejak pojistit*/
        return;
    }

    for (i = 0; i < mat->cols; ++i) {
        matrix_set(mat, row, i, matrix_get(mat, row, i) / divider);
    }
}

void _matrix_pivoting(struct matrix *mat, size_t row, size_t col) {
    double multiplier;
    size_t i, j;

    if (!mat || !mat->items || row >= mat->rows || col >= mat->cols) {
        return;
    }

    _matrix_normalize_row_by_col(mat, row, col);

    for (i = 0; i < mat->rows; ++i) {
        if (i == row) {
            continue;
        }

        multiplier = matrix_get(mat, i, col);

        for (j = 0; j < mat->cols; ++j) {
            matrix_set(mat, i, j, matrix_get(mat, i, j) - multiplier * matrix_get(mat, row, j));
        }
    }
}

int _smallest_quotient_row_index(const struct matrix *mat, size_t col, size_t *row_to_optimize) {
    size_t i, have_first_candidate;
    double min_value, test;

    if (!mat || !mat->items || col >= mat->cols || mat->rows == 0) {
        return -1;
    }

    have_first_candidate = 0;

    for (i = 0; i < mat->rows; ++i) {
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
        return 1;
    }
    else {
        return 0;
    }
}

int optimal_max_test(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, size_t *col_to_optimize) {
    mat_num_type max_value, test_value;
    size_t col_index, row_index;

    if (!mat || !mat->items || !base_vars || !object_to || !col_to_optimize) {
        return -1;
    }

    max_value = 0.;
    for (col_index = 0; col_index < mat->cols - 1; ++col_index) {
        test_value = object_to[col_index];
        for (row_index = 0; row_index < mat->rows; ++row_index) {
            test_value -= matrix_get(mat, row_index, col_index) * object_to[base_vars[row_index]];
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

int simplex_maximize(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, mat_num_type *result, size_t real_vars_count) {
    size_t smallest_quotient_row, smallest_quotient_col, row_index;
    
    if (!mat || !mat->items || !base_vars || !object_to) {
        return -2;
    }

    /* while (!_is_last_row_nonnegative(mat)) {
        smallest_quotient_col = _smallest_col_index(mat, mat->rows - 1);
        smallest_quotient_row = _smallest_quotient_row_index(mat, smallest_quotient_col);
        _matrix_pivoting(mat, smallest_quotient_row, smallest_quotient_col);
    } */

   while (!optimal_max_test(mat, base_vars, object_to, &smallest_quotient_col)) {
        if (_smallest_quotient_row_index(mat, smallest_quotient_col, &smallest_quotient_row)) {
            return 10; /* TODO: tady to znamená že jsem nedostal žádnou pozitivní thetu */
        }
        base_vars[smallest_quotient_row] = smallest_quotient_col;

        printf("Pivot row: %lu, Pivot col: %lu\n", smallest_quotient_row, smallest_quotient_col);
        matrix_print(mat);
        _matrix_pivoting(mat, smallest_quotient_row, smallest_quotient_col);

        for (row_index = 0; row_index < mat->rows; ++row_index) {
            if (base_vars[row_index] >= real_vars_count) {
                continue;
            }
            result[base_vars[row_index]] = matrix_get(mat, row_index, mat->cols - 1);
        }
    }

    return 0;


}

void simplex_minimize(struct matrix *mat) {

}

/* int main() {
    struct matrix *mat;
    mat = matrix_allocate(3, 6, 0.0);
    matrix_set(mat, 0, 0, 1.0);
    matrix_set(mat, 0, 1, 1.0);
    matrix_set(mat, 0, 2, 1.0);
    matrix_set(mat, 0, 3, 0.0);
    matrix_set(mat, 0, 4, 0.0);
    matrix_set(mat, 0, 5, 12.0);
    matrix_set(mat, 1, 0, 2.0);
    matrix_set(mat, 1, 1, 1.0);
    matrix_set(mat, 1, 2, 0.0);
    matrix_set(mat, 1, 3, 1.0);
    matrix_set(mat, 1, 4, 0.0);
    matrix_set(mat, 1, 5, 16.0);
    matrix_set(mat, 2, 0, -40.0);
    matrix_set(mat, 2, 1, -30.0);
    matrix_set(mat, 2, 2, 0.0);
    matrix_set(mat, 2, 3, 0.0);
    matrix_set(mat, 2, 4, 1.0);
    matrix_set(mat, 2, 5, 0.0);
    matrix_print(mat);
    simplex_maximize(mat);
    matrix_print(mat);
    matrix_free(&mat);
    return 0;
} */