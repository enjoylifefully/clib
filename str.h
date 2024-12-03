#ifndef STR_H
#define STR_H

#include "int.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef char *cstr;

// to do
typedef struct {
  cstr data;
  usize size;
  usize capacity;
} str;

typedef struct {
  char *data;
  usize size;
} str_view;

typedef struct {
  str_view *data;
  usize size;
  usize capacity;
} sv_collection;

#define SV_FMT "%.*s"
#define SV_ARG(sv) (i32)(sv).size, (sv).data
#define SV_END -1
#define SV_START 0
#define SV_NOT_FOUND -1
#define SV_EMPTY ((str_view){.data = "", .size = 0})
#define SV_COLLEC_EMPTY                                                        \
  ((sv_collection){.data = nullptr, .size = 0, .capacity = 0})

#define sv_static(s) ((str_view){.data = (s), .size = sizeof(s) - 1})
#define sv_with_size(s, sz) ((str_view){.data = (s), .size = (sz)})
#define sv_from_cstr(s) ((str_view){.data = (s), .size = strlen(s)})

bool sv_first(str_view sv, char *ch);

void sv_mut_slice(str_view *sv, usize start, isize end);

static inline str_view sv_slice(str_view sv, usize start, isize end) {
  sv_mut_slice(&sv, start, end);
  return sv;
}

#define sv_slice_cstr(s, start, end) sv_slice(sv_static(s), start, end);

void sv_mut_shift(str_view *sv);
static inline str_view sv_shift(str_view sv) {
  sv_mut_shift(&sv);
  return sv;
}

void sv_mut_skip(str_view *sv, usize count);
static inline str_view sv_skip(str_view sv, usize count) {
  sv_mut_skip(&sv, count);
  return sv;
}

void sv_mut_trim(str_view *sv);
static inline str_view sv_trim(str_view sv) {
  sv_mut_trim(&sv);
  return sv;
}

void sv_mut_trim_left(str_view *sv);
static inline str_view sv_trim_left(str_view sv) {
  sv_mut_trim_left(&sv);
  return sv;
}

void sv_mut_trim_right(str_view *sv);
static inline str_view sv_trim_right(str_view sv) {
  sv_mut_trim_right(&sv);
  return sv;
}

bool sv_equal(str_view sv_a, str_view sv_b);

#define sv_equal_cstr(sv, s) sv_equal(sv, sv_static(s))

bool sv_starts_with(str_view sv, str_view pattern);

#define sv_starts_with_cstr(sv, pattern) sv_starts_with(sv, sv_static(pattern))

bool sv_ends_with(str_view sv, str_view pattern);

#define sv_ends_with_cstr(sv, pattern) sv_ends_with(sv, sv_static(pattern))

usize sv_find_char(str_view sv, char to_find);

usize sv_find(str_view sv, str_view to_find);

#define sv_find_cstr(sv, to_find) sv_find(sv, sv_static(to_find))

void sv_mut_take_while(str_view *sv, int (*predicate)(int c));
static inline str_view sv_take_while(str_view sv, int (*predicate)(int c)) {
  sv_mut_take_while(&sv, predicate);
  return sv;
}

void sv_mut_skip_while(str_view *sv, int (*predicate)(int c));

static inline str_view sv_skip_while(str_view sv, int (*predicate)(int c)) {
  sv_mut_skip_while(&sv, predicate);
  return sv;
}

bool sv_splitn(str_view sv, str_view pattern, sv_collection *collec, usize n);

#define sv_splitn_cstr(sv, pattern, collec, n)                                 \
  sv_splitn(sv, sv_static(pattern), collec, n)

static inline bool sv_split(str_view sv, str_view pattern,
                            sv_collection *collec) {
  return sv_splitn(sv, pattern, collec, USIZE_MAX);
}

#define sv_split_cstr(sv, pattern, collec)                                     \
  sv_split(sv, sv_static(pattern), collec)

bool sv_splitn_if(str_view sv, int (*predicate)(int c), sv_collection *collec,
                  usize n);

static inline bool sv_split_if(str_view sv, int (*predicate)(int c),
                               sv_collection *collec) {
  return sv_splitn_if(sv, predicate, collec, USIZE_MAX);
};

static inline bool sv_is_empty(str_view sv) { return sv.size == 0; }

bool sv_to_u64(str_view sv, u64 *out);

cstr sv_to_owned_cstr(str_view sv);

usize sv_match(str_view sv, cstr first, ...);

#define sv_switch(sv)                                                          \
  for (str_view __sv = (sv); __sv.data != nullptr; __sv.data = nullptr)

#define sv_case(s) if (sv_equal_cstr(__sv, s))

#define sv_default()

bool sv_collec_push(sv_collection *collec, str_view sv);
bool sv_collec_grow(sv_collection *collec);

void sv_collec_free(sv_collection *collec);

#endif
