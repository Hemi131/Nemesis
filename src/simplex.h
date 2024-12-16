#include "matrix.h"

/**
 * \brief Procedura provede normalizaci řádku `row` matice `mat` podle sloupce `col`.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param row Index řádku matice.
 * \param col Index sloupce matice.
 */
void _matrix_normalize_row_by_col(struct matrix *mat, size_t row, size_t col);

/**
 * \brief Procedura provede pivotaci matice `mat` podle řádku `row` a sloupce `col`.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param row Index řádku matice.
 * \param col Index sloupce matice.
 */
void _matrix_pivoting(struct matrix *mat, size_t row, size_t col);


/**
 * \brief Funkce zjistí, jestli je poslední řádka matice `matrix` kladná nebo alespoň neutrální.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \return int 1, pokud je poslední řádka matice nenegativní, jinak 0.
 */
int _is_last_row_nonnegative(const struct matrix *mat);

/**
 * \brief Funkce vrátí index sloupce s nejmenší hodnotou v řádce `row` matice `mat`.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param row Index řádku matice.
 * \return size_t Index sloupce s nejmenší hodnotou v poslední řádce matice.
 */
int _smallest_col_index(const struct matrix *mat, size_t row);

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

/**
 * \brief Funkce provede maximalizaci simplexové tabulky.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 */
int simplex_maximize(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, mat_num_type *result, size_t real_vars_count);

/**
 * \brief Funkce provede minimalizaci simplexové tabulky.
 * \param mat Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 */
void simplex_minimize(struct matrix *mat);