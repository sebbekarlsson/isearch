#ifndef ISEARCH_H
#define ISEARCH_H
#include <isearch/node.h>
#include <stdbool.h>
#include <bytepager/bytepager.h>
#include <isearch/query.h>
#include <isearch/string.h>
#include <arena/arena.h>
#include <hashy/hashy.h>
#include <stdint.h>
#include <stdio.h>

#define ISEARCH_STRING_ALLOCATOR_MAX_PAGE_TRAVERSAL 160000
#define ISEARCH_STRING_ALLOCATOR_PAGE_CAPACITY 9600
#define ISEARCH_CACHE_ALLOCATOR_ITEMS_PER_PAGE 500

typedef struct {
  int tmp;
} ISearchConfig;

typedef struct {
  ISearchConfig config;
  ISearchNodeBuffer nodes;
  Bytepager string_allocator;
  ISearchStringBuffer words;
  
  Arena cache_allocator;
  HashyMap query_cache;
  
  bool initialized;
} ISearch;

int isearch_init(ISearch *s, ISearchConfig cfg);
int isearch_destroy(ISearch *s);
int isearch_purge_cache(ISearch* s);

int isearch_add_word(ISearch *s, const char *word);
int isearch_add_node(ISearch* s, ISearchNode node);
int isearch_build_association_tree(ISearch *s);

ISearchNode *isearch_get_node_by_id(ISearch *s, uint64_t id);
int isearch_search(ISearch* s, ISearchQuery query, ISearchStringBuffer* out);

int isearch_print(ISearch *s, FILE *fp);

int isearch_serialize(ISearch s, FILE* fp);
int isearch_deserialize(ISearch* s, FILE* fp);

#endif
