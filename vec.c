#ifndef VEC_C
#define VEC_C

#include "vec.h"
#include <stdlib.h>
#include <string.h>

bool __vec_grow(vector *vec, usize elem_size) {
  if ((vec->size + 1) * elem_size <= vec->capacity)
    return true;
  usize new_capacity = vec->capacity == 0 ? 2 * elem_size : vec->capacity * 2;
  byte *new_data = realloc(vec->data, new_capacity);
  if (new_data == nullptr)
    return false;
  vec->data = new_data;
  vec->capacity = new_capacity;
  return true;
}

bool __vec_push(vector *vec, void *elem, usize elem_size) {
  if (!__vec_grow(vec, elem_size))
    return false;
  memcpy(vec->data + vec->size * elem_size, elem, elem_size);
  vec->size += 1;
  return true;
}

void vec_free(vector *vec) {
  free(vec->data);
  vec->data = nullptr;
  vec->size = 0;
  vec->capacity = 0;
}

#endif
