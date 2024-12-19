/* https://brilliant.org/wiki/shunting-yard-algorithm/ */

#include "matrix.h"
#include "stack.h"
#include "queue.h"

#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

#define EXIT_INVALID_INPUT_FILE 1
#define EXIT_INVALID_OUTPUT_FILE 2

#define MAX_EXPRESSION_LENGTH 200

/**
 * \brief Funkce provede parsování argumentů příkazové řádky.
 * \param argc Počet argumentů.
 * \param argv Pole argumentů.
 * \param input_file Ukazatel na řetězec, do kterého bude uložen název vstupního souboru.
 * \param output_file Ukazatel na řetězec, do kterého bude uložen název výstupního souboru.
 * \return int 1, pokud proběhla v pořádku, jinak 0.
 */
int args_parser(int argc, char *argv[], char **input_file, char **output_file);

/**
 * \brief Funkce odstraní všechny výskyty podřetězce ze zadaného řetězce.
 * \param str Řetězec.
 * \param substr Podřetězec.
 * \return int 1, pokud proběhla v pořádku, jinak 0.
 */
int remove_substr(char *str, const char *substr);

/**
 * \brief Funkce zkontroluje validitu znaků v řetězci.
 * \param str Řetězec.
 * \return int 1, pokud je validní, jinak 0.
 */
int check_valid_chars(const char *str);

/**
 * \brief Funkce nahradí všechny výskyty podřetězce za jiný podřetězec.
 * \param str Řetězec.
 * \param substr Podřetězec.
 * \param replacement Nahrazující podřetězec.
 * \return int 1, pokud proběhla v pořádku, jinak 0.
 */
int replace_substr(char *str, const char *substr, const char *replacement);

int replace_substr_with_end(char *str, const char *substr);
/**
 * \brief Seřadí pole stringů podle délky sesestupně.
 * \param *str  Pole řetězce.
 * \param count Počet řetězců v poli.
 */
void sort_str_by_len(char **str, size_t count);

/**
 * \brief Funkce vymění název proměnné za její index v poli povolených proměnných.
 * \param str Řetězec.
 */
void replace_vars_by_index(char *str, char **vars, const size_t vars_count);

/**
 * \brief Funkce zkontroluje a přípraví výraz k parsování do RPN.
 * \param str Řetězec.
 * \return int 1, pokud je správně a připraven, jinak 0.
 */
int prepare_expression(char *str, char **vars, const size_t vars_count);

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
 * \brief Inicializuje prvek RPN typu number.
 * \param number Hodnota čísla.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
struct rpn_item rpn_item_create_number(mat_num_type number);

/**
 * \brief Inicializuje prvek RPN typu variable.
 * \param variable Index proměnné v poli povolených proměnných.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
struct rpn_item rpn_item_create_variable(size_t variable);

/**
 * \brief Inicializuje prvek RPN typu operator.
 * \param operator Operátor.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
struct rpn_item rpn_item_create_operator_first_level(char operator);

/**
 * \brief Inicializuje prvek RPN typu operator.
 * \param operator Operátor.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
struct rpn_item rpn_item_create_operator_second_level(char operator);

/**
 * \brief Inicializuje prvek RPN typu operator.
 * \param bracket Závorka.
 * \return struct rpn_item* Ukazatel na nově vytvořený prvek RPN.
 */
struct rpn_item rpn_item_create_bracket(char bracket);

#define MAX_VAR_COUNT 10

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
 * \param var_count Počet proměnných výrazu.
 * \param constant Konstanta výrazu.
 * \return struct evaluation_expression Výraz
 */
struct evaluation_expression create_evaluation_expression_constant(const mat_num_type constant);

/**
 * \brief Funkce provede inicializaci výrazu pro vyhodnocení typu proměnná.
 * \param var_count Počet proměnných výrazu.
 * \param variable_index Index proměnné v poli povolených proměnných.
 * \return struct evaluation_expression Výraz.
 */
struct evaluation_expression create_evaluation_expression_variable(const size_t var_count, const size_t variable_index);

/**
 * \brief Funkce sečte dva výrazy, vrátí nov.
 * \param expr1 První výraz.
 * \param expr2 Druhý výraz.
 * \return struct evaluation_expression Výsledná výraz.
 */
struct evaluation_expression add_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);


/**
 * \brief Funkce vynásobí dva výrazy.
 * Funkce předpokládá, že druhý výraz je pouze konstanta !!!
 * \param expr1 První výraz.
 * \param expr2 Druhý výraz.
 * \return struct evaluation_expression Výsledný výraz.
 */
struct evaluation_expression multiply_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);


/**
 * \brief Funkce odečte dva výrazy.
 * \param expr1 První výraz.
 * \param expr2 Druhý výraz.
 * \return struct evaluation_expression Výsledný výraz.
 */
struct evaluation_expression sub_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);

/**
 * \brief Funkce vydělí dva výrazy, vrátí nový výraz.
 * Funkce předpokládá, že druhý výraz je pouze konstanta !!!
 * \param expr1 První výraz.
 * \param expr2 Druhý výraz.
 * \return struct evaluation_expression Výsledný výraz.
 */
struct evaluation_expression divide_evaluation_expressions(struct evaluation_expression expr1, struct evaluation_expression expr2);

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
 * \brief Funkce provede převod infixový výraz na reverzní polskou notaci pomocí Shunting Yard algoritmu.
 * \param str Ukazatel na řetězec, který bude převeden na RPN.
 * \return struct queue* Ukazatel na frontu s RPN výrazem.
 */
struct queue *parse_to_rpn(const char *str);

/**
 * \brief Funkce provede vyhodnocení RPN výrazu.
 * \param rpn Ukazatel na frontu s RPN výrazem.
 * \return evaluation_expression Výsledek vyhodnocení.
 */
struct evaluation_expression rpn_evaluate(struct queue *rpn, const size_t var_count);

#endif