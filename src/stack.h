/**
 * \file stack.h
 * \author František Pártl (fpartl@kiv.zcu.cz)
 * \brief Hlavičkový soubor s deklaracemi funkcí pracujících se zásobníkovou strukturou.
 * \version 1.0
 * \date 2020-09-08
 */

#ifndef STACK_H
#define STACK_H

#include <stddef.h>

/** \brief Definice zásobníkové struktury s možností vložení libovolně velkého prvku. */
struct stack {
    /* TODO */

    size_t capacity;    /**< Maximální počet prvků zásobníku. */
    size_t item_size;   /**< Velikost jednoho prvku zásobníku. */

    size_t sp;      /**< Index vrcholu zásobníku. */

    void *items;         /**< Ukazatel na oblast paměti, kde jsou uloženy prvky zásobníku. */
};

#define DEFAULT_STACK {0, 0, 0, NULL}

/**
 * \brief Funkce dynamicky alokuje instanci struktury `stack`, kterou inicializuje pomocí funkce `stack_init`.
 * \param capacity Maximální počet položek zásobníku.
 * \param item_size Velikost jednoho prvku zásobníku.
 * \return stack* Ukazatel na nově dynamicky alokovanou instanci struktury `stack`, nebo `NULL` při chybě.
 */
struct stack *stack_alloc(const size_t capacity, const size_t item_size);

/**
 * \brief Funkce pro inicializaci instance struktury `stack`.
 * \param s Ukazatel na inicializovanou instanci struktury `stack`.
 * \param capacity Maximální počet položek zásobníku.
 * \param item_size Velikost jednoho prvku zásobníku.
 * \return int 1, pokud inicializace zásobníku proběhla v pořádku, jinak 0.
 */
int stack_init(struct stack *s, const size_t capacity, const size_t item_size);

/**
 * \brief Funkce provede korektní uvolnění členů instance struktury `stack`.
 * \param s Ukazatel na instanci struktury `stack` jejichž členy budou korektně uvolněny.
 */
void stack_deinit(struct stack *s);

/**
 * \brief Funkce uvolní dynamicky alokovanou instanci struktury `stack`. K uvolnění vnitřních členů
 *        struktury používá funkce `stack_deinit`. Ukazatel, na který `s` ukazuje bude nastaven na
 *        hodnotu `NULL`, tzn. neplatný ukazatel.
 * \param s Ukazatel na ukazatel na instanci struktury `stack`, která bude uvolněna.
 */
void stack_dealloc(struct stack **s);

/**
 * \brief Funkce vloží nový prvek na adrese `item` do zásobníku, který je dán ukazatelem `s` na instanci struktury `stack`.
 * \param s Ukazatel na instanci struktury `stack`, do které bude vložen nový záznam.
 * \param item Ukazatel na vkládaný prvek.
 * \return int 1 pokud vložení (kopírování) prvku do zásobníku dopadlo dobře, jinak 0.
 */
int stack_push(struct stack *s, const void *item);

/**
 * \brief Funkce odebere prvek z vrcholu zásobníku daného ukazatelem `s`. Odebraný prvek bude zkopírován do oblasti paměti,
 *        dané ukazatelem `item` (pokud je předaný ukazatel nenulový).
 * \param s Ukazatel na instanci struktury `stack`, ze které bude prvek odebrán.
 * \param item Ukazatel na oblast paměti, kam bude odebraný prvek zkopírován.
 * \return int 1, pokud vše dopadlo dobře, jinak 0.
 */
int stack_pop(struct stack *s, void *item);

/**
 * \brief Funkce zkopíruje prvek z vrcholu zásobníku do oblasti paměti dané ukazatel `item`.
 * \param s Ukazatel na instanci struktury `stack`, jejíž poslední přidaný prvek budeme kopírovat.
 * \param item Ukazatel na oblast paměti, kam se přečtený prvek zkopíruje.
 * \return int 1, pokud kopírování dopadlo dobře, jinak 0.
 */
int stack_head(const struct stack *s, void *item);

/**
 * \brief Funkce vrátí počet prvků v zásobníku zadaného ukazatelem `s`.
 * \param s Ukazatel na instanci struktury `stack`.
 * \return size_t Počet prvků v zásobníku.
 */
size_t stack_item_count(const struct stack *s);

#endif
