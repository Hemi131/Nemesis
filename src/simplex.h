#include "matrix.h"
#include "parser.h"

#define EXIT_OBJECTIVE_UNBOUNDED 20
#define EXIT_OBJECTIVE_INFEASIBLE 21

#define UNUSED_VAR 1

#define THE_BIG_M 1000.0
#define MAX_ITERATIONS_COUNT 100

/**
 * \brief Procedura provede normalizaci řádku `row` matice `mat` podle sloupce `col`.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param row Index řádku matice.
 * \param col Index sloupce matice.
 */
int _matrix_normalize_row_by_col(struct matrix *mat, size_t row, size_t col);

/**
 * \brief Procedura provede pivotaci matice `mat` podle řádku `row` a sloupce `col`.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param row Index řádku matice.
 * \param col Index sloupce matice.
 */
int _matrix_pivoting(struct matrix *mat, size_t row, size_t col);

/**
 * \brief Funkce vrátí index řádku s nejmenším kvocientem.
 *       Kvocient je určen jako podíl hodnoty v posledním sloupci a hodnoty v zadaném sloupci.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param col Index sloupce matice.
 * \return size_t Index řádku s nejmenším kvocientem.
 */
int _smallest_quotient_row_index(const struct matrix *mat, size_t col, size_t *row_to_optimize);

/**
 * \brief Funkce provede test optimality.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param base_vars Pole indexů bázových proměnných.
 * \param object_to Pole hodnot cílové funkce.
 * \param col_to_optimize Ukazatel na index sloupce, který bude optimalizován.
 * \return int 1, pokud je test optimality splněn, jinak 0.
 */
int optimal_max_test(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, size_t *col_to_optimize);

int simplex_valid_base(struct matrix *mat, size_t *base_vars, size_t not_valid_basis_count, size_t *not_valid_basis);

/**
 * \brief Funkce provede maximalizaci simplexové tabulky.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 */
int simplex_maximize(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, mat_num_type *result, size_t real_vars_count, size_t *not_valid_basis, size_t not_valid_basis_count);

int simplex(struct problem_data *data);