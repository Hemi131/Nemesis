/**
 * \file queue.h
 * \author Filip Nehéz (filipn@students.zcu.cz)
 * \brief Hlavičkový soubor s deklaracemi funkcí pracujících s frontovou strukturou.
 * \version 1.0
 * \date 2024-12-03
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

/** \brief Definice frontové struktury s možností vložení prvků libovolné velikosti. */
struct queue {
    size_t capacity;    /**< Maximální počet prvků ve frontě. */
    size_t item_size;   /**< Velikost jednoho prvku ve frontě. */

    size_t front;       /**< Index předního prvku ve frontě. */
    size_t rear;        /**< Index zadního prvku ve frontě. */
    size_t count;       /**< Aktuální počet prvků ve frontě. */

    void *items;        /**< Ukazatel na oblast paměti, kde jsou uloženy prvky fronty. */
};

#define DEFAULT_QUEUE {0, 0, 0, 0, 0, NULL}

/**
 * \brief Funkce dynamicky alokuje instanci struktury `queue`, kterou inicializuje pomocí funkce `queue_init`.
 * \param capacity Maximální počet položek ve frontě.
 * \param item_size Velikost jednoho prvku ve frontě.
 * \return queue* Ukazatel na nově dynamicky alokovanou instanci struktury `queue`, nebo `NULL` při chybě.
 */
struct queue *queue_alloc(const size_t capacity, const size_t item_size);

/**
 * \brief Funkce pro inicializaci instance struktury `queue`.
 * \param q Ukazatel na inicializovanou instanci struktury `queue`.
 * \param capacity Maximální počet položek ve frontě.
 * \param item_size Velikost jednoho prvku ve frontě.
 * \return int 1, pokud inicializace fronty proběhla v pořádku, jinak 0.
 */
int queue_init(struct queue *q, const size_t capacity, const size_t item_size);

/**
 * \brief Funkce provede korektní uvolnění členů instance struktury `queue`.
 * \param q Ukazatel na instanci struktury `queue`, jejichž členy budou korektně uvolněny.
 */
void queue_deinit(struct queue *q);

/**
 * \brief Funkce uvolní dynamicky alokovanou instanci struktury `queue`. K uvolnění vnitřních členů
 *        struktury používá funkce `queue_deinit`. Ukazatel, na který `q` ukazuje, bude nastaven na
 *        hodnotu `NULL`, tzn. neplatný ukazatel.
 * \param q Ukazatel na ukazatel na instanci struktury `queue`, která bude uvolněna.
 */
void queue_dealloc(struct queue **q);

/**
 * \brief Funkce vloží nový prvek na adresu `item` do fronty dané ukazatelem `q` na instanci struktury `queue`.
 * \param q Ukazatel na instanci struktury `queue`, do které bude vložen nový záznam.
 * \param item Ukazatel na vkládaný prvek.
 * \return int 1, pokud vložení (kopírování) prvku do fronty proběhlo v pořádku, jinak 0.
 */
int queue_enqueue(struct queue *q, const void *item);

/**
 * \brief Funkce odstraní prvek z přední části fronty dané ukazatelem `q`. Odstraněný prvek bude zkopírován do paměťové oblasti
 *        dané ukazatelem `item` (pokud je předaný ukazatel nenulový).
 * \param q Ukazatel na instanci struktury `queue`, ze které bude prvek odstraněn.
 * \param item Ukazatel na paměťovou oblast, kam bude odstraněný prvek zkopírován.
 * \return int 1, pokud vše proběhlo v pořádku, jinak 0.
 */
int queue_dequeue(struct queue *q, void *item);

/**
 * \brief Funkce zkopíruje prvek z přední části fronty do paměťové oblasti dané ukazatelem `item`.
 * \param q Ukazatel na instanci struktury `queue`, jejíž přední prvek bude zkopírován.
 * \param item Ukazatel na paměťovou oblast, kam bude přečtený prvek zkopírován.
 * \return int 1, pokud kopírování proběhlo v pořádku, jinak 0.
 */
int queue_front(const struct queue *q, void *item);

/**
 * \brief Funkce vrátí počet prvků ve frontě dané ukazatelem `q`.
 * \param q Ukazatel na instanci struktury `queue`.
 * \return size_t Počet prvků ve frontě.
 */
size_t queue_item_count(const struct queue *q);

void queue_print(const struct queue *q);

#endif