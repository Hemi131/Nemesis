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
        printf("Division by zero.\n"); /* #TODO: testovaci vypis, odebrat!!!!!!!!! */
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

int _is_last_row_nonnegative(const struct matrix *mat)
{
    size_t i;

    if (!mat || !mat->items || mat->rows == 0) {
        return 0;
    }

    for (i = 0; i < mat->cols; ++i) {
        if (matrix_get(mat, mat->rows - 1, i) < 0) {
            return 0;
        }
    }

    return 1;
}

int _smallest_col_index(const struct matrix *mat, size_t row) {
    size_t i, min_index;
    double min_value, test;

    if (!mat || !mat->items || row >= mat->rows || mat->cols == 0)
        return -1; /* TODO: testovat to pak po pouziti neb nejak predelat. */

    min_index = 0;
    min_value = matrix_get(mat, row, 0);
    for (i = 1; i < mat->cols; ++i) { /* TODO: psat tam meyerz nebo ne mey mat a cols? */
        test = matrix_get(mat, row, i);
        if (test < min_value) {
            min_index = i;
            min_value = test;
        }
    }

    return min_index;
}

int _smallest_quotient_row_index(const struct matrix *mat, size_t col) {
    size_t i, min_index;
    double min_value, test;

    if (!mat || !mat->items || col >= mat->cols || mat->rows == 0) {
        return -1;
    }

    min_index = 0;
    min_value = matrix_get(mat, 0, mat->cols - 1) / matrix_get(mat, 0, col);

    for (i = 1; i < mat->rows - 1; ++i) {
        test = matrix_get(mat, i, mat->cols - 1) / matrix_get(mat, i, col);
        if (test < min_value) {
            min_index = i;
            min_value = test;
        }
    }

    return min_index;
}

void simplex_maximize(struct matrix *mat) {
    size_t smallest_quotient_row, smallest_quotient_col;
    
    if (!mat || !mat->items) {
        return;
    }

    while (!_is_last_row_nonnegative(mat)) {
        smallest_quotient_col = _smallest_col_index(mat, mat->rows - 1);
        smallest_quotient_row = _smallest_quotient_row_index(mat, smallest_quotient_col);
        _matrix_pivoting(mat, smallest_quotient_row, smallest_quotient_col);
    }
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