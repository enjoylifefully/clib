#ifndef STR_C
#define STR_C

#include "str.h"
#include "int.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

bool sv_first(str_view sv, char *ch) {
  if (sv.size == 0)
    return false;
  *ch = sv.data[0];
  return true;
}

void sv_mut_slice(str_view *sv, usize start, isize end) {
  if (end < 0)
    end += sv->size + 1;
  if (end < 0)
    *sv = SV_EMPTY;
  else {
    if (end > sv->size)
      end = sv->size;
    if (start >= (usize)end)
      *sv = SV_EMPTY;
    else {
      sv->data += start;
      sv->size = end - start;
    }
  }
}

void sv_mut_shift(str_view *sv) {
  if (sv->size != 0) {
    sv->data += 1;
    sv->size -= 1;
  }
}

void sv_mut_skip(str_view *sv, usize count) {
  if (count <= sv->size) {
    sv->data += count;
    sv->size -= count;
  }
}

void sv_mut_trim(str_view *sv) {
  usize start = 0, end = sv->size;
  while (start < sv->size && isspace(sv->data[start]))
    start += 1;
  while (end > start && isspace(sv->data[end - 1]))
    end -= 1;
  sv->data += start;
  sv->size = end - start;
}

void sv_mut_trim_left(str_view *sv) {
  usize start = 0;
  while (start < sv->size && isspace(sv->data[start]))
    start += 1;
  sv->data += start;
  sv->size -= start;
}

void sv_mut_trim_right(str_view *sv) {
  while (sv->size > 0 && isspace(sv->data[sv->size - 1]))
    sv->size -= 1;
}

bool sv_equal(str_view sv_a, str_view sv_b) {
  if (sv_a.size != sv_b.size)
    return false;
  return memcmp(sv_a.data, sv_b.data, sv_a.size) == 0;
}

bool sv_starts_with(str_view sv, str_view pattern) {
  if (sv.size < pattern.size)
    return false;
  return memcmp(sv.data, pattern.data, pattern.size) == 0;
}

bool sv_ends_with(str_view sv, str_view pattern) {
  if (sv.size < pattern.size)
    return false;
  return memcmp(sv.data + sv.size - pattern.size, pattern.data, pattern.size) ==
         0;
}

usize sv_find_char(str_view sv, char to_find) {
  for (usize i = 0; i < sv.size; i += 1)
    if (sv.data[i] == to_find)
      return i;
  return SV_NOT_FOUND;
}

usize sv_find(str_view sv, str_view to_find) {
  if (sv.size < to_find.size)
    return SV_NOT_FOUND;
  if (to_find.size == 0)
    return 0;
  if (to_find.size == 1)
    return sv_find_char(sv, to_find.data[0]);
  for (usize pos = 0; pos < sv.size; pos += 1) {
    if (sv.data[pos] == to_find.data[0] &&
        memcmp(sv.data + pos, to_find.data, to_find.size) == 0)
      return pos;
  }
  return SV_NOT_FOUND;
}

void sv_mut_take_while(str_view *sv, int (*predicate)(int c)) {
  usize i = 0;
  while (i < sv->size && predicate(sv->data[i]))
    i += 1;
  sv->size = i;
}

void sv_mut_skip_while(str_view *sv, int (*predicate)(int c)) {
  while (sv->size > 0 && predicate(sv->data[0])) {
    sv->data += 1;
    sv->size -= 1;
  }
}

bool sv_splitn(str_view sv, str_view pattern, sv_collection *collec, usize n) {
  if (sv.size == 0) {
    collec->size = 0;
    return true;
  }
  usize found = sv_find(sv, pattern);
  for (; n > 1; n -= 1) {
    if (found == SV_NOT_FOUND)
      break;
    str_view to_push = sv_with_size(sv.data, found);
    if (!sv_collec_push(collec, to_push))
      return false;
    do {
      usize offset = found + pattern.size;
      sv.data += offset;
      sv.size -= offset;
      found = sv_find(sv, pattern);
    } while (found == 0);
  }
  if (!sv_collec_push(collec, sv))
    return false;
  return true;
}

bool sv_splitn_if(str_view sv, int (*predicate)(int c), sv_collection *collec,
                  usize n) {
  if (sv.size == 0) {
    collec->size = 0;
    return true;
  }
  for (; n > 1; n -= 1) {
    str_view to_push = sv_with_size(sv.data, 0);
    while (sv.size != 0 && !predicate(sv.data[0])) {
      to_push.size += 1;
      sv.data += 1;
      sv.size -= 1;
    }
    if (!sv_collec_push(collec, to_push))
      return false;
    if (sv.size == 0)
      return true;
    while (sv.size != 0 && predicate(sv.data[0])) {
      sv.data += 1;
      sv.size -= 1;
    }
  }
  if (!sv_collec_push(collec, sv))
    return false;
  return true;
}

bool sv_to_u64(str_view sv, u64 *out_value) {
  u64 value = 0;
  for (usize i = 0; i < sv.size; i += 1) {
    if (!isdigit(sv.data[i]))
      return false;
		if (__builtin_umulll_overflow(value, 10, &value))
			return false;
		if (__builtin_uaddll_overflow(value, sv.data[i] - '0', &value))
			return false;
  }
  *out_value = value;
  return true;
}

cstr sv_to_owned_cstr(str_view sv) {
  cstr buffer = malloc(sv.size + 1);
  if (buffer == nullptr)
    return nullptr;
  memcpy(buffer, sv.data, sv.size);
  buffer[sv.size] = '\0';
  return buffer;
}

usize sv_match(str_view sv, cstr first, ...) {
  if (sv_equal_cstr(sv, first))
    return 1;
  usize return_val = SV_NOT_FOUND;
  va_list args;
  va_start(args, first);
  cstr curr = va_arg(args, cstr);
  for (usize i = 2; curr != nullptr; i += 1) {
    if (sv_equal_cstr(sv, curr))
      return_defer(i);
    curr = va_arg(args, cstr);
  }
defer:
  va_end(args);
  return return_val;
}

bool sv_collec_push(sv_collection *collec, str_view sv) {
  if (!sv_collec_grow(collec))
    return false;
  collec->data[collec->size] = sv;
  collec->size += 1;
  return true;
}

bool sv_collec_grow(sv_collection *collec) {
  if (collec->size + 1 <= collec->capacity)
    return true;
  usize new_capacity = collec->capacity == 0 ? 2 : collec->capacity * 2;
  str_view *new_data = realloc(collec->data, new_capacity * sizeof(str_view));
  if (new_data == nullptr)
    return false;
  collec->data = new_data;
  collec->capacity = new_capacity;
  return true;
}

void sv_collec_free(sv_collection *collec) {
  free(collec->data);
  collec->data = nullptr;
  collec->size = 0;
  collec->capacity = 0;
}

#endif
