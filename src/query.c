#include <isearch/query.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isearch_query_to_string(ISearchQuery query, char *out, int64_t capacity) {
  if (!query.word || capacity <= 0 || out == 0)
    return 0;
  snprintf(out, capacity, "%s_%d_%s_%s", query.word, query.max_distance,
           query.must_contain ? query.must_contain : "?",
           query.must_not_contain ? query.must_not_contain : "?");
  return 1;
}
