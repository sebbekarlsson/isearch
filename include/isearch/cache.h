#ifndef ISEARCH_CACHE_H
#define ISEARCH_CACHE_H
#include <isearch/string.h>
#include <arena/arena.h>
typedef struct {
  ISearchStringBuffer words;
  ArenaRef ref;
} ISearchCacheItem;

void isearch_cache_item_destructor(ISearchCacheItem* item);
#endif
