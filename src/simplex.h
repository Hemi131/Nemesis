/**
 * \file simplex.h
 * \author Filip Nehéz (filipn@students.zcu.cz)
 * \brief Hlavičkový soubor s deklaracemi funkcí pro simplexová algoritmus.
 * \version 1.0
 * \date 2024-12-23
 */

#include "matrix.h"
#include "parser.h"

#ifndef SIMPLEX_H
#define SIMPLEX_H

#define EXIT_OBJECTIVE_UNBOUNDED 20
#define EXIT_OBJECTIVE_INFEASIBLE 21

#define UNUSED_VAR 1

#define THE_BIG_M 1000.0
#define MAX_ITERATIONS_COUNT 100

/**
 * \brief Funkce provede normalizaci řádku `row` matice `mat` podle sloupce `col`.
 * \param `*mat` Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param `row` Index řádku matice.
 * \param `col` Index sloupce matice.
 * \return int 1, pokud proběhla v pořádku, jinak 0.
 */
int _matrix_normalize_row_by_col(struct matrix *mat, size_t row, size_t col);

/**
 * \brief Funkce provede pivotaci matice `mat` podle řádku `row` a sloupce `col`.
 * \param `*mat` Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param `row` Index řádku matice.
 * \param `col` Index sloupce matice.
 * \return int 1, pokud proběhla v pořádku, jinak 0.
 */
int _matrix_pivoting(struct matrix *mat, size_t row, size_t col);

/**
 * \brief Funkce vrátí index řádku s nejmenším koeficientem.
 * Koeficient je určen jako podíl hodnoty v posledním sloupci (konstanta, pravá strana rovnice) a hodnoty v zadaném sloupci.
 * \param `*mat` Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param `col` Index sloupce matice.
 * \param `*row_to_optimize` Ukazatel na size_t proměnnou, kam bude uložen index řádku pro optimalizaci.
 * \return int 1, pokud byl nalezen, jinak 0.
 */
int _smallest_quotient_row_index(const struct matrix *mat, size_t col, size_t *row_to_optimize);

/**
 * \brief Funkce provede test optimality a nalezne sloupec pro optimalizaci.
 * \param `*mat` Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param `base_vars` Pole indexů bázových proměnných.
 * \param `object_to` Pole koeficientů proměnných cílové funkce.
 * \param `col_to_optimize` Ukazatel na size_t proměnnou, kam bude uložen index sloupce pro optimalizaci.
 * \return int 1, pokud je test optimality splněn, jinak 0.
 */
int optimal_max_test(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, size_t *col_to_optimize);

/**
 * \brief Funkce provede kontrolu validní báze.
 * \param `*mat` Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param `*base_vars` Pole indexů bázových proměnných.
 * \param `not_valid_basis_count` Počet nevalidních bázových proměnných.
 * \param `*not_valid_basis` Pole indexů nevalidních bázových proměnných.
 * \return int 1, pokud je báze validní, jinak 0.
 */
int simplex_valid_base(struct matrix *mat, size_t *base_vars, size_t not_valid_basis_count, size_t *not_valid_basis);

/**
 * \brief Funkce provede maximalizaci simplexové tabulky.
 * \param `*mat` Ukazatel na instanci struktury `matrix`, nad kterou bude operace provedena.
 * \param `*base_vars` Pole indexů bázových proměnných.
 * \param `*object_to` Pole koeficientů proměnných cílové funkce.
 * \param `*result` Pole výsledků.
 * \param `real_vars_count` Počet reálných proměnných.
 * \param `*not_valid_basis` Pole indexů nevalidních bázových proměnných.
 * \param `not_valid_basis_count` Počet nevalidních bázových proměnných.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak kód chyby.
 */
int simplex_maximize(struct matrix *mat, size_t *base_vars, mat_num_type *object_to, mat_num_type *result, size_t real_vars_count, size_t *not_valid_basis, size_t not_valid_basis_count);

/**
 * \brief Funkce provede simplexovou metodu.
 * Tady jsem poprvé pochopil simplex: https://math.libretexts.org/Bookshelves/Applied_Mathematics/Applied_Finite_Mathematics_(Sekhon_and_Bloom)/04%3A_Linear_Programming_The_Simplex_Method/4.02%3A_Maximization_By_The_Simplex_Method
 * Pak tady: https://www.youtube.com/watch?v=UaELzREcX0Q
 * \param `*data` Ukazatel na strukturu `problem_data`.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak kód chyby.
 */
int simplex(struct problem_data *data);

#endif