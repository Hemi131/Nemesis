#include "vector.h"

#include <stdlib.h>
#include <string.h>

/**
 * \brief Pracuje stejně jako funkce vector_at (viz vector.h), ale neprovádí kontrolu rozsahu přistupovaného vektoru.
 *        Funkce je deklarovaná jako statická, tj. její viditelnost je omezena na objektový soubor. Vytvořili jsme
 *        tedy takové typově bezpečné makro. Jak jsme viděli na druhém cvičení, optimalizátor funkci celkem jistě
 *        "inlinuje", takže se nemusíme bát drahého funkčního volání.
 *
 *        Statickou funkci podlé této jmenné konvence poznám podle podtržítka na začátku jejího názvu. Pozor ale na použití
 *        dvou podtržítek! Jazyk C nemá jmenné prostory a názvy začínající dvěma podtržítky označují interní funkce 
 *        standardní knihovny. Může se vám tedy stát, že váš dvojitě opodtržítkovaný kód jednou přestane fungovat,
 *        protože někdo do standardní knihovny přidá funkci, která se bude jmenovat stejně jako ta vaše a konflikt je světě.
 * \param v Ukazatel na přistupovanou instanci struktury `vector`.
 * \param at Index prvku ve vektoru.
 * \return void* Ukazatel na at-tý prvek vektoru.
 */
static void *_vector_at(const struct vector *v, const size_t at) {
    return (char *)v->data + (at * v->item_size);
}

struct vector *vector_allocate(const size_t item_size, const vec_it_dealloc_type deallocator) {
    struct vector *new;

    if (item_size == 0) {
        return NULL;
    }

    /* V čistém Céčku nikdy neprovádějte explicitní konverzi `void *`. Je to zbytečné a skryjete tím potenciální chyby.*/
    /* Akorát se v kódu stále opakujete, a to obecně není dobrý nápad. */
    /* `new = (struct vector *)malloc(sizeof(struct vector));` */
    /* buď `new = malloc(sizeof(struct vector));` nebo */
    new = malloc(sizeof(*new));
    if (!new) {
        return NULL;
    }

    if (!vector_init(new, item_size, deallocator)) {
        free(new);
        return NULL;
    }

    return new;
}

int vector_init(struct vector *v, const size_t item_size, const vec_it_dealloc_type deallocator) {
    if (!v || item_size == 0) {
        return 0;
    }

    v->item_size = item_size;
    v->deallocator = deallocator;
    v->capacity = 0;
    v->data = NULL;
    v->count = 0;

    if (!vector_realloc(v, VECTOR_INIT_SIZE)) {
        return 0;
    }

    return 1;
}

void vector_deinit(struct vector *v) {
    size_t i;

    if (!v) {
        return;
    }

    if (v->deallocator) {
        for (i = 0; i < vector_count(v); i++) {
            v->deallocator(_vector_at(v, i));
        }
    }

    if (v->data) {
        free(v->data);
        v->data = NULL;
    }
    v->count = 0;
    v->capacity = 0;
    v->item_size = 0;
    v->deallocator = NULL;
}

void vector_deallocate(struct vector **v)
{
    if (!v || !*v) {
        return;
    }

    vector_deinit(*v);

    free(*v);
    *v = NULL;
}

size_t vector_capacity(const struct vector *v) {
    return v ? v->capacity : 0; 
}

size_t vector_count(const struct vector *v) {
    return v ? v->count : 0;
}

int vector_isempty(const struct vector *v)  {
    return vector_count(v) == 0;
}

int vector_realloc(struct vector *v, const size_t capacity) {
    void *new_data;

    if (!v || capacity < vector_count(v)) {
        return 0;
    }

    new_data = realloc(v->data, capacity * v->item_size);

    if (!new_data) {
        return 0;
    }

    v->data = new_data;
    v->capacity = capacity;

    return 1;
}

void *vector_at(const struct vector *v, const size_t at) {
    if (!v || at > vector_count(v) - 1) {
        return NULL;
    }

    return _vector_at(v, at);
}

int vector_push_back(struct vector *v, const void *item) {
    if (!v || !item) {
        return 0;
    }

    if (vector_count(v) >= vector_capacity(v)) {
        if (!vector_realloc(v, vector_capacity(v) * VECTOR_SIZE_MULT)) {
            return 0;
        }
    }
    
    memcpy(_vector_at(v, vector_count(v)), item, v->item_size);
    v->count++;
    
    return 1;
}

void *vector_give_up(struct vector *v) {
    void *data;

    if (vector_isempty(v)) {
        return NULL;
    }

    data = v->data;

    vector_init(v, v->item_size, v->deallocator);

    return data;
}
