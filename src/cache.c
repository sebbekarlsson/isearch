#include <isearch/cache.h>
#include <isearch/macros.h>

void isearch_cache_item_destructor(ISearchCacheItem *item) {
  if (!item)
    return;

  isearch_ISearchString_buffer_clear(&item->words);
}
