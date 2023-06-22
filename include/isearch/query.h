#ifndef ISEARCH_QUERY_H
#define ISEARCH_QUERY_H
#include <stdint.h>
typedef struct {
  const char* word;
  const char* must_contain;
  const char* must_not_contain;
  int max_distance;
} ISearchQuery;

int isearch_query_to_string(ISearchQuery query, char* out, int64_t capacity);
#endif
