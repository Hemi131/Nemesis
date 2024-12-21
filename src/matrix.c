/**
 * \file matrix.c
 * \brief Soubor obsahující implementace operací se strukturou matrix.
 * \version 1.0
 * \date 2020-09-07
 */

#include "matrix.h"

#include <stdlib.h>
#include <stdio.h>

/* TODO */

struct matrix *matrix_allocate(const size_t rows, const size_t cols, const mat_num_type i_val){
    struct matrix *new_mat;

    if (rows == 0 || cols == 0) {
        return NULL;
    }

    new_mat = malloc(sizeof(struct matrix));
    if (!new_mat) {
        return NULL;
    }

    if (!matrix_init(new_mat, rows, cols, i_val)) {
        free(new_mat);
        return NULL;
    }

    return new_mat;
}

int matrix_init(struct matrix *mat, const size_t rows, const size_t cols, const mat_num_type i_val){
    if (!mat || rows == 0 || cols == 0) {
        return 0;
    }

    mat->rows = rows;
    mat->cols = cols;

    mat->items = malloc(rows * cols * sizeof(mat_num_type));

    if (!mat->items) {
        return 0;
    }

    matrix_fill(mat, i_val);

    return 1;
}

void matrix_deinit(struct matrix *mat) {
    if (!mat) {
        return;
    }

    mat->rows = 0;
    mat->cols = 0;

    if (mat->items) {
        free(mat->items);
        mat->items = NULL;
    }
}
void matrix_free(struct matrix **mat) {
    if (!mat || !*mat) {
        return;
    }

    matrix_deinit(*mat);
    free(*mat);
    *mat = NULL;
}

void matrix_fill(struct matrix *mat, const mat_num_type val) {
    size_t i;

    if (!mat || !mat->items) {
        return;
    }

    for (i = 0; i < mat->cols * mat->rows; ++i) {
        mat->items[i] = val;
    }
}

void matrix_print(const struct matrix *mat) {
    size_t r, c;

    if (!mat || !mat->items) {
        printf("| NULL |\n");
        return;
    }

    for (r = 0; r < mat->rows; ++r) {
        printf("| ");

        for (c = 0; c < mat->cols; ++c) {
            printf(MAT_NUMBER_FORMAT, matrix_get(mat, r, c));
        }

        printf("|\n");
    }

    printf("\n");
}

mat_num_type matrix_get(const struct matrix *mat, const size_t row, const size_t col)
{
    return mat->items[row * mat->cols + col];
}

void matrix_set(struct matrix *mat, const size_t row, const size_t col, mat_num_type val)
{
    if (!mat || !mat->items)
        return;
    mat->items[row * mat->cols + col] = val;
}

struct matrix *matrix_add(const struct matrix *a, const struct matrix *b) {
    struct matrix *temp;
    size_t i;

    /* Tahle podmínka kontroluje správnost argumentů z pohledu jazyka. */
    if (!a || !a->items || !b || !b->items) {
        return NULL;
    }

    /* Další podmínka pak řeší správnost argumentů z pohledu řešené úlohy. */
    if (a->rows != b->rows || a->cols != b->cols) {
        return NULL;
    }

    temp = matrix_allocate(a->rows, a->cols, 0);
    if (!temp) {
        return NULL;
    }

    for (i = 0; i < a->rows * a->cols; ++i) {
        temp->items[i] = a->items[i] + b->items[i];
    }

    return temp;
}

struct matrix *matrix_mul(const struct matrix *a, const struct matrix *b) {
    struct matrix *temp;
    size_t ar, bc, ac;
    mat_num_type mult_temp;

    if (!a || !a->items || !b || !b->items) {
        return NULL;
    }

    if (a->cols != b->rows) {
        return NULL;
    }

    temp = matrix_allocate(a->rows, b->cols, 0.0);
    if (!temp) {
        return NULL;
    }

    for (ar = 0; ar < a->rows; ++ar) {
        for (bc = 0; bc < b->cols; ++bc) {
            mult_temp = 0;

            for (ac = 0; ac < a->cols; ++ac)
                mult_temp += matrix_get(a, ar, ac) * matrix_get(b, ac, bc);

            matrix_set(temp, ar, bc, mult_temp);
        }
    }

    return temp;
}

void matrix_scale(struct matrix *mat, const mat_num_type scaler) {
    size_t i;

    if (!mat || !mat->items) {
        return;
    }

    for (i = 0; i < mat->cols * mat->rows; ++i)
        mat->items[i] *= scaler;
}

void matrix_move(struct matrix **target, struct matrix **source) {
    if (!target || !source) {
        return;
    }

    if (*target) {
        matrix_free(target);
    }

    *target = *source;
    *source = NULL;
}

void matrix_transpose(struct matrix *mat) {
    mat_num_type temp;
    size_t r, c;

    if (!mat || !mat->items) {
        return;
    }

    if (mat->rows != mat->cols) {
        return;
    }

    for (r = 0; r < mat->rows; ++r) {
        for (c = 0; c < mat->cols; ++c) {
            temp = matrix_get(mat, r, c);
            matrix_set(mat, r, c, matrix_get(mat, c, r));
            matrix_set(mat, c, r, temp);
        }
    }
}

int is_column_zero(const struct matrix *mat, size_t col) {
    size_t i;

    if (!mat || !mat->items || col >= mat->cols) {
        return 0;
    }

    for (i = 0; i < mat->rows; ++i) {
        if (matrix_get(mat, i, col) != 0.0) {
            return 0;
        }
    }

    return 1;
}
