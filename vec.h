#ifndef VEC_H
#define VEC_H

#include "int.h"
#include <stdbool.h>

typedef struct {
  byte *data;
  usize size;
  usize capacity;
} vector;

#define VEC_EMPTY ((vector){.data = nullptr, .size = 0, .capacity = 0})

#define vector(type) vector

#define vec_for_each(type, vec, name)                                          \
  for (type *name = (type *)(vec)->data;                                       \
       (byte *)name < (vec)->data + (vec)->size * sizeof(type); name += 1)

#define vec_push(type, vec, elem) __vec_push(vec, elem, sizeof(type))

#define vec_push_val(type, vec, elem)                                          \
  ({                                                                           \
    type _e = elem;                                                            \
    __vec_push(vec, &_e, sizeof(type));                                        \
  })

// fazer capacity guardar o numero de bytes inves da quantidade de elemntos para
// que nao ocorra acesso indevido
bool __vec_grow(vector *vec, usize elem_size);

bool __vec_push(vector *vec, void *elem, usize elem_size);

void vec_free(vector *vec);

#endif
