/* https://brilliant.org/wiki/shunting-yard-algorithm/ */

#include "matrix.h"
#include "stack.h"
#include "queue.h"
#include "vector.h"

#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

/**
 * \brief Funkce porovnává délky dvou řetězců.
 * \param str1 První řetězec.
 * \param str2 Druhý řetězec.
 * \return int -1, pokud je první řetězec kratší, 0 pokud jsou stejně dlouhé, 1 pokud je první řetězec delší.
 */
int strlencmp(const char *str1, const char *str2);

/**
 * \brief Funkce odstraní všechny výskyty podřetězce ze zadaného řetězce.
 * \param str Řetězec.
 * \param substr Podřetězec.
 * \return int 1, pokud proběhla v pořádku, jinak 0.
 */
int remove_substr(char *str, const char *substr);

/**
 * \brief Struktura pro reprezentaci prvku v reverzní polské notaci.
 * Prvek může být číslo, proměnná nebo operátor.
 * 'd' - number, 'v' - variable, '1' - operator level 1 , '2' - operator level 2, 'l' - left bracket, 'r' - right bracket
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
 * \brief Funkce vytvoří nový prvek RPN typu number.
 * \param number Hodnota čísla.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
int rpn_item_create_number(struct rpn_item *rpn_item, mat_num_type number);

/**
 * \brief Funkce vytvoří nový prvek RPN typu variable.
 * \param variable Index proměnné v poli povolených proměnných.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
int rpn_item_create_variable(struct rpn_item *rpn_item, size_t variable);

/**
 * \brief Funkce vytvoří nový prvek RPN typu operator.
 * \param operator Operátor.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
int rpn_item_create_operator_first_level(struct rpn_item *rpn_item, char operator);

/**
 * \brief Funkce vytvoří nový prvek RPN typu operator.
 * \param operator Operátor.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
int rpn_item_create_operator_second_level(struct rpn_item *rpn_item, char operator);

/**
 * \brief Funkce vytvoří nový prvek RPN typu operator.
 * \param bracket Závorka.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
int rpn_item_create_bracket(struct rpn_item *rpn_item, char bracket);

/**
 * \brief Struktura pro vyhodnocení výrazu.
 * Obsahuje konstantu a koeficienty proměnných.
 * Lze sčítat, odčítat, násobit a dělit.
 */
struct evaluation_expression {
    mat_num_type constant;
    mat_num_type *var_koeficients;
    size_t var_count;
};


/**
 * \brief Funkce provede alokaci nového výrazu pro vyhodnocení.
 * \param var_count Počet proměnných výrazu.
 * \param constant Konstanta výrazu.
 * \return struct evaluation_expression* Ukazatel na nově alokovaný výraz.
 */
struct evaluation_expression *create_evaluation_expression_constant(const size_t var_count, const mat_num_type constant);

/**
 * \brief Funkce provede alokaci nového výrazu pro vyhodnocení.
 * \param var_count Počet proměnných výrazu.
 * \param variable_index Index proměnné v poli povolených proměnných.
 * \return struct evaluation_expression* Ukazatel na nově alokovaný výraz.
 */
struct evaluation_expression *create_evaluation_expression_variable(const size_t var_count, const size_t variable_index);

/**
 * \brief Funkce uvolní alokovaný výraz.
 * \param expr Ukazatel na ukazatel na uvolňovaný výraz.
 */
void free_evaluation_expression(struct evaluation_expression **expr);

/**
 * \brief Funkce sečte dva výrazy, vrátí nový výraz.
 * \param var_count Počet proměnných výrazu.
 * \param constant Konstanta výrazu.
 * \param variable_index Index proměnné v poli povolených proměnných.
 * \return struct evaluation_expression* Ukazatel na nově alokovaný výraz.
 */
struct evaluation_expression *add_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2);


/**
 * \brief Funkce vynásobí dva výrazy, vrátí nový výraz.
 * Funkce předpokládá, že druhý výraz je pouze konstanta !!!
 * \param expr1 První výraz.
 * \param expr2 Druhý výraz.
 * \return struct evaluation_expression* Ukazatel na nově vytvořený výraz.
 */
struct evaluation_expression *multiply_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2);


/**
 * \brief Funkce odečte dva výrazy, vrátí nový výraz.
 * \param expr1 První výraz.
 * \param expr2 Druhý výraz.
 * \return struct evaluation_expression* Ukazatel na nově vytvořený výraz.
 */
struct evaluation_expression *sub_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2);

/**
 * \brief Funkce vydělí dva výrazy, vrátí nový výraz.
 * Funkce předpokládá, že druhý výraz je pouze konstanta !!!
 * \param expr1 První výraz.
 * \param expr2 Druhý výraz.
 * \return struct evaluation_expression* Ukazatel na nově vytvořený výraz.
 */
struct evaluation_expression *divide_evaluation_expressions(struct evaluation_expression *expr1, struct evaluation_expression *expr2);

/**
 * \brief Funkce provede kontrolu syntaxe závorek v zadaném řetězci.
 * \param str Ukazatel na řetězec, ve kterém budou kontrolovány závorky.
 * Vkládat pouze stringy vytvořené mallocem!!! TODO: možná odebrat
 * \return int 1, pokud jsou závorky správně uzavřeny, jinak 0.
 */
int check_brackets(const char *str);

/**
 * \brief Funkce provede změnu závorek v zadaném řetězci.
 * Závorky '{', '[' budou změněny na '(' a závorky '}', ']' budou změněny na ')'.
 * Vkládat pouze stringy vytvořené mallocem!!! TODO: možná odebrat
 * \param str Ukazatel na řetězec, ve kterém budou závorky změněny.
 * \return char* Ukazatel na nově vytvořený řetězec s novými závorkami.
 */
int change_brackets(char *str);

/**
 * \brief Funkce doplní '*' mezi číslo a proměnnou, mezi závorku a proměnnou, mezi číslo a závorku.
 * \param str Ukazatel na řetězec, ve kterém přidány '*'.
 * \param allowed_variables Ukazatel na vektor povolených proměnných.
 * \return int 1, pokud se akce provedla, jinak 0.
 */
int amend_multiplication_stars(char *str, const struct vector *allowed_variables);

/**
 * \brief Funkce provede převod infixový výraz na reverzní polskou notaci pomocí Shunting Yard algoritmu.
 * \param str Ukazatel na řetězec, který bude převeden na RPN.
 * \return struct queue* Ukazatel na frontu s RPN výrazem.
 */
struct queue *parse_to_rpn(const char *str);

/**
 * \brief Funkce provede vyhodnocení RPN výrazu.
 * \param rpn Ukazatel na frontu s RPN výrazem.
 * \return evaluation_expression* Ukazatel na výsledek vyhodnocení.
 */
struct evaluation_expression *rpn_evaluate(const struct queue *rpn);

#endif