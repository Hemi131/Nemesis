/**
 * \file parser.h
 * \author Filip Nehéz (filipn@students.zcu.cz)
 * \brief Hlavičkový soubor s deklaracemi funkcí pro parsing vstupního souboru a jeho zpracování pro simplexový algoritmus.
 * \version 1.0
 * \date 2024-12-23
 */

#include "matrix.h"
#include "stack.h"
#include "queue.h"

#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#define EXIT_INVALID_INPUT_FILE 1
#define EXIT_INVALID_OUTPUT_FILE 2 
#define EXIT_UNKNOWN_VAR 10
#define EXIT_SYNTAX_ERROR 11
#define EXIT_MALLOC_ERROR 1001

#define MAX_EXPRESSION_LENGTH 200
#define MAX_CHARS 256
#define MAX_LINES 20
#define MAX_VAR_COUNT 10

#define MAXIMIZE 1
#define MINIMIZE -1

/**
 * \brief Funkce provede parsování argumentů příkazové řádky.
 * \param `argc` Počet argumentů.
 * \param `*argv[]` Pole argumentů.
 * \param `**input_file` Ukazatel na ukazatel na řetězec, do kterého bude uložen název vstupního souboru.
 * \param `**output_file` Ukazatel na ukazatel na řetězec, do kterého bude uložen název výstupního souboru.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak kód chyby.
 */
int args_parser(int argc, char *argv[], char **input_file, char **output_file);

/**
 * \brief Funkce odstraní všechny výskyty podřetězce ze zadaného řetězce.
 * \param `*str` Řetězec.
 * \param `*substr` Podřetězec.
 * \return int 1, pokud proběhla v pořádku, jinak 0.
 */
int remove_substr(char *str, const char *substr);

/**
 * \brief Funkce zkontroluje, zda může být řetězec proměnnou.
 * \param `*str` Řetězec.
 * \return int 1, pokud může být proměnnou, jinak 0.
 */
int can_be_var(const char *str);

/**
 * \brief Funkce zkontroluje validitu znaků v řetězci.
 * \param `*str` Řetězec.
 * \param `*unknown_var` Ukazatel na řetězec, do kterého bude uložen neznámý výraz.
 * \return int 1, pokud je validní, jinak 0.
 */
int check_valid_chars(const char *str, char *unknown_var);

/**
 * \brief Funkce nahradí všechny výskyty podřetězce za jiný podřetězec.
 * Původní řetězec je změněn, proto musí být alokován dostatečný prostor !!!
 * Inspirováno: https://www.geeksforgeeks.org/find-and-replace-all-occurrence-of-a-substring-in-the-given-string/
 * \param `*str` Řetězec.
 * \param `*substr` Podřetězec.
 * \param `*replacement` Nahrazující podřetězec.
 */
void replace_substr(char *str, const char *substr, const char *replacement);

/**
 * \brief Funkce nahradí první výskyt podřetězce za ukončovací znak.
 * \param `*str` Řetězec.
 * \param `*substr` Podřetězec.
 */
void replace_substr_with_end(char *str, const char *substr);

/**
 * \brief Seřadí pole stringů podle délky sesestupně.
 * \param `*str`  Pole řetězce.
 * \param `count` Počet řetězců v poli.
 */
void sort_str_by_len(char **str, size_t count);

/**
 * \brief Funkce vymění název proměnné za její index v poli povolených proměnných ohraničený složenými závorkami.
 * \param `*str` Řetězec.
 * \param `**vars` Pole povolených proměnných.
 * \param `vars_count` Počet povolených proměnných.
 */
void replace_vars_by_index(char *str, char **vars, const size_t vars_count);

/**
 * \brief Funkce zkontroluje a přípraví výraz k parsování do RPN.
 * \param `*str` Řetězec.
 * \param `**vars` Pole povolených proměnných.
 * \param `vars_count` Počet povolených proměnných.
 * \param `*unknown_var` Ukazatel na řetězec, do kterého bude uložen neznámý výraz.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak kód chyby.
 */
int prepare_expression(char *str, char **vars, const size_t vars_count, char *unknown_var);

/**
 * \brief Struktura pro reprezentaci prvku v reverzní polské notaci.
 * Prvek může být číslo, proměnná nebo operátor.
 * 'd' - number, 'v' - variable, '1' - operator level 1 , '2' - operator level 2, 'b' - bracket
 */
struct rpn_item {
    char type; /* 'd' - number, 'v' - variable, '1' - operator level 1 , '2' - operator level 2, 'b' - bracket */
    union rpn_item_data {
        mat_num_type number;
        size_t variable;  /* index of variable in vector of allowed variables */
        char operator;
        char bracket;
    } data;
};
/**
 * \brief Inicializuje prvek RPN typu number.
 * \param `number` Hodnota čísla.
 * \return `struct rpn_item` Struktura rpn_item.
 */
struct rpn_item rpn_item_create_number(mat_num_type number);

/**
 * \brief Inicializuje prvek RPN typu variable.
 * \param `variable` Index proměnné v poli povolených proměnných.
 * \return `struct rpn_item` Struktura rpn_item.
 */
struct rpn_item rpn_item_create_variable(size_t variable);

/**
 * \brief Inicializuje prvek RPN typu operator.
 * \param `operator` Operátor.
 * \return `struct rpn_item` Struktura rpn_item.
 */
struct rpn_item rpn_item_create_operator_first_level(char operator);

/**
 * \brief Inicializuje prvek RPN typu operator.
 * \param operator Operátor.
 * \return `struct rpn_item` Struktura rpn_item.
 */
struct rpn_item rpn_item_create_operator_second_level(char operator);

/**
 * \brief Inicializuje prvek RPN typu operator.
 * \param bracket Závorka.
 * \return `struct rpn_item` Struktura rpn_item.
 */
struct rpn_item rpn_item_create_bracket(char bracket);

/**
 * \brief Struktura pro vyhodnocení výrazu.
 * Obsahuje konstantu a koeficienty proměnných.
 * Lze sčítat, odčítat, násobit a dělit.
 * Násobení a dělení pouze konstantou !!!!!!
 */
struct evaluation_expression {
    mat_num_type constant;
    mat_num_type var_koeficients[MAX_VAR_COUNT];
    size_t var_count;
};

/**
 * \brief Funkce provede inicializaci výrazu pro vyhodnocení typu konstanta.  
 * \param `constant` Konstanta výrazu.
 * \return `struct evaluation_expression` Výraz
 */
struct evaluation_expression create_evaluation_expression_constant(const mat_num_type constant);

/**
 * \brief Funkce provede inicializaci výrazu pro vyhodnocení typu proměnná.
 * \param `var_count` Počet proměnných výrazu.
 * \param `variable_index` Index proměnné v poli povolených proměnných.
 * \return `struct evaluation_expression` Výraz.
 */
struct evaluation_expression create_evaluation_expression_variable(const size_t var_count, const size_t variable_index);

/**
 * \brief Funkce sečte dva výrazy, vrátí nov.
 * \param `expr1` První výraz.
 * \param `expr2` Druhý výraz.
 * \return `struct evaluation_expression` Výsledný výraz.
 */
struct evaluation_expression add_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);


/**
 * \brief Funkce vynásobí dva výrazy.
 * Funkce předpokládá, že druhý výraz je pouze konstanta !!!
 * \param `expr1` První výraz.
 * \param `expr2` Druhý výraz.
 * \return `struct evaluation_expression` Výsledný výraz.
 */
struct evaluation_expression multiply_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);


/**
 * \brief Funkce odečte dva výrazy.
 * \param `expr1` První výraz.
 * \param `expr2` Druhý výraz.
 * \return `struct evaluation_expression` Výsledný výraz.
 */
struct evaluation_expression sub_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);

/**
 * \brief Funkce vydělí dva výrazy, vrátí nový výraz.
 * Funkce předpokládá, že druhý výraz je pouze konstanta !!!
 * \param `expr1` První výraz.
 * \param `expr2` Druhý výraz.
 * \return `struct evaluation_expression` Výsledný výraz.
 */
struct evaluation_expression divide_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);

/**
 * \brief Funkce provede kontrolu syntaxe závorek v zadaném řetězci.
 * \param `*str` Ukazatel na řetězec, ve kterém budou kontrolovány závorky.
 * \return int 1, pokud jsou závorky správně uzavřeny, jinak 0.
 */
int check_brackets(const char *str);

/**
 * \brief Funkce provede změnu závorek v zadaném řetězci.
 * Závorky '{', '[' budou změněny na '(' a závorky '}', ']' budou změněny na ')'.
 * \param str Ukazatel na řetězec, ve kterém budou závorky změněny.
 */
void change_brackets(char *str);

/**
 * \brief Funkce provede převod infixového výrazu na reverzní polskou notaci pomocí Shunting Yard algoritmu.
 * Tady jsem pochopil tento algoritmus: https://brilliant.org/wiki/shunting-yard-algorithm/
 * \param `*str` Ukazatel na řetězec, který bude převeden na RPN.
 * \return `struct queue*` Ukazatel na frontu s RPN výrazem.
 */
struct queue *parse_to_rpn(const char *str);

/**
 * \brief Funkce provede vyhodnocení RPN výrazu.
 * \param `*rpn` Ukazatel na frontu s RPN výrazem. Tato fronta bude po použití uvolněna
 * \param `var_count` Počet proměnných.
 * \param `*exprResult` Ukazatel na již alokovanou strukturu, kam bude výsledek uložen.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak EXIT_FAILURE.
 */
int rpn_evaluate(struct queue *rpn, const size_t var_count, struct evaluation_expression* exprResult);

/**
 * \brief Struktura pro reprezentaci dat problému.
 * Obsahuje typ problému, počet podmínek, počet omezení, počet povolených proměnných, výraz cílové funkce, výrazy podmínek a omezení, pole povolených proměnných, pole operátorů pro podmínky a omezení, pole nepoužitých proměnných a výsledek.
 */
struct problem_data {
    int problem_type;
    size_t subjects_count, bounds_count, allowed_vars_count;
    struct evaluation_expression purpose_expr, *subjects_expr, *bounds_expr;
    char **allowed_vars;
    int *subjects_op, *bounds_op, *unused_vars;
    mat_num_type *result;
};

/**
 * \brief Funkce alokuje instanci struktury `problem_data` a inicializuje ji.
 * \param `var_count` Počet povolených proměnných.
 * \param `subjects_count` Počet podmínek.
 * \param `bounds_count` Počet omezení.
 * \param `problem_type` Typ problému.
 * \return `struct problem_data*` Ukazatel na instanci struktury `problem_data`.
 */
struct problem_data *problem_data_alloc(const size_t var_count, const size_t subjects_count, const size_t bounds_count, const int problem_type);

/**
 * \brief Funkce inicializuje instanci struktury `problem_data`.
 * \param `*data` Ukazatel na instanci struktury `problem_data`.
 * \param `var_count` Počet povolených proměnných.
 * \param `subjects_count` Počet podmínek.
 * \param `bounds_count` Počet omezení.
 * \param `problem_type` Typ problému.
 * \return int 1, pokud inicializace proběhla v pořádku, jinak 0.
 */
int problem_data_init(struct problem_data *data, const size_t var_count, const size_t subjects_count, const size_t bounds_count, const int problem_type);

/**
 * \brief Funkce provede deinicializaci instance struktury `problem_data`.
 * \param `*data` Ukazatel na instanci struktury `problem_data`.
 */
void problem_data_deinit(struct problem_data *data);

/**
 * \brief Funkce provede korektní uvolnění členů instance struktury `problem_data`.
 * \param `*data` Ukazatel na instanci struktury `problem_data`, která bude uvolněna.
 */
void problem_data_dealloc(struct problem_data *data);

/**
 * \brief Funkce provede parsování vstupního souboru.
 * \param `*input_file` Ukazatel na řetězec s cestou k vstupnímu souboru.
 * \param `**problem_data` Ukazatel na ukazatel na strukturu `problem_data`, kde bude alokována nová instance této struktury.
 * \param `*unknown_var` Ukazatel na řetězec, do kterého bude uložen neznámý výraz.
 * \return EXIT_SUCCESS, pokud proběhlo v pořádku, jinak kód chyby.
 */
int input_parser(char *input_file, struct problem_data **problem_data, char *unknown_var);

#endif