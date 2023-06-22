#include <arena/arena.h>
#include <arena/config.h>
#include <bytepager/bytepager.h>
#include <bytepager/config.h>
#include <hashy/hashy.h>
#include <isearch/cache.h>
#include <isearch/isearch.h>
#include <isearch/macros.h>
#include <isearch/node.h>
#include <isearch/query.h>
#include <isearch/string.h>
#include <isearch/uint64.h>
#include <limits.h>
#include <linux/limits.h>
#include <mif/utils.h>
#include <stdio.h>
#include <stdlib.h>

int isearch_init(ISearch *s, ISearchConfig cfg) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);

  if (s->initialized)
    return 1;

  s->config = cfg;

  isearch_ISearchNode_buffer_init(&s->nodes);
  isearch_ISearchString_buffer_init(&s->words);

  bytepager_init(
      &s->string_allocator,
      (BytepagerConfig){
          .max_page_traversal = ISEARCH_STRING_ALLOCATOR_MAX_PAGE_TRAVERSAL,
          .page_capacity_bytes =
              ISEARCH_STRING_ALLOCATOR_PAGE_CAPACITY * sizeof(char)});

  arena_init(
      &s->cache_allocator,
      (ArenaConfig){.free_function =
                        (ArenaFreeFunction)isearch_cache_item_destructor,
                    .item_size = sizeof(ISearchCacheItem),
                    .items_per_page = ISEARCH_CACHE_ALLOCATOR_ITEMS_PER_PAGE});

  hashy_map_init(&s->query_cache, ISEARCH_CACHE_ALLOCATOR_ITEMS_PER_PAGE);

  s->initialized = true;
  return 1;
}

int isearch_destroy(ISearch *s) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);

  for (int64_t i = 0; i < s->nodes.length; i++) {
    isearch_node_destroy(&s->nodes.items[i]);
  }

  isearch_ISearchNode_buffer_clear(&s->nodes);
  isearch_ISearchString_buffer_clear(&s->words);
  hashy_map_clear(&s->query_cache, false);
  arena_destroy(&s->cache_allocator);
  bytepager_destroy(&s->string_allocator);
  s->initialized = false;
  s->config = (ISearchConfig){0};

  return 1;
}
int isearch_purge_cache(ISearch *s) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);
  hashy_map_clear(&s->query_cache, false);
  arena_unuse_all(&s->cache_allocator);
  arena_clear(&s->cache_allocator);
  return 1;
}
int isearch_add_word(ISearch *s, const char *word) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);
  if (word == 0)
    return 0;

  ISearchString sstring = {0};
  sstring.value = bytepager_strdup(&s->string_allocator, word);
  sstring.length = strlen(sstring.value);

  sstring.id = (uint64_t)s->words.length;
  ISEARCH_ASSERT_RETURN(sstring.value != 0, 0);
  isearch_ISearchString_buffer_push(&s->words, sstring);

  return 1;
}
int isearch_add_node(ISearch *s, ISearchNode node) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);

  node.id = (uint64_t)s->nodes.length;
  isearch_ISearchNode_buffer_push(&s->nodes, node);

  return 1;
}

static int isearch_generate_nodes(ISearch *s) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);

  if (s->words.length <= 0)
    return 0;
  ISEARCH_ASSERT_RETURN(s->words.items != 0, 0);

  for (int64_t i = 0; i < s->words.length; i++) {
    ISearchString word = s->words.items[i];
    ISearchNode node = (ISearchNode){.word = word};
    isearch_add_node(s, node);
  }

  return s->nodes.length > 0 && s->nodes.items != 0;
}

static int isearch_associate_node(ISearch *s, ISearchNode *node) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);
  ISEARCH_ASSERT_RETURN(node != 0, 0);
  ISEARCH_ASSERT_RETURN(node->word.value != 0, 0);
  ISEARCH_ASSERT_RETURN(s->nodes.length > 0, 0);
  // if (node->children.length > 0) return 1;

  uint64_t best_id = 0;
  int min_distance = INT_MAX;
  bool found = false;

  for (int64_t i = 0; i < s->nodes.length; i++) {
    ISearchNode b = s->nodes.items[i];
    ISEARCH_ASSERT_RETURN(b.word.value != 0, 0);
    if (b.id == node->id || b.word.id == node->word.id)
      continue;
    if (b.children.length > 0)
      continue;

    int distance = mif_lev_fast(node->word.value, b.word.value);

    if (distance < min_distance) {
      min_distance = distance;
      best_id = b.id;
      found = true;
    }
  }

  if (found) {
    if (!isearch_node_add_child(node, best_id))
      return 0;
  }

  return found == true;
}
int isearch_build_association_tree(ISearch *s) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);

  if (s->words.length <= 0)
    return 0;
  ISEARCH_ASSERT_RETURN(s->words.items != 0, 0);

  if (s->nodes.length > 0) {
    isearch_ISearchNode_buffer_clear(&s->nodes);
  }

  if (!isearch_generate_nodes(s)) {
    ISEARCH_WARNING_RETURN(0, stderr, "Failed to generate nodes.\n");
  }

  for (int64_t i = 0; i < s->nodes.length; i++) {
    ISearchNode *node = &s->nodes.items[i];
    isearch_associate_node(s, node);
  }

  return 1;
}

ISearchNode *isearch_get_node_by_id(ISearch *s, uint64_t id) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);
  if (s->nodes.length <= 0 || s->nodes.items == 0)
    return 0;

  ISEARCH_ASSERT_RETURN(id >= 0, 0);
  ISEARCH_ASSERT_RETURN(id < s->nodes.length, 0);

  return &s->nodes.items[id];
}

static void isearch_print_node(ISearch *s, ISearchNode node, int pad,
                               FILE *fp) {
  ISEARCH_PAD_PRINT(pad, "Node {");
  fprintf(fp, "\n");
  ISEARCH_PAD_PRINTF(pad + 2, "id: %ld", node.id);
  fprintf(fp, "\n");
  ISEARCH_PAD_PRINTF(pad + 2, "word: \"%s\"",
                     node.word.value ? node.word.value : "?");
  fprintf(fp, "\n");

  if (node.children.length > 0 && node.children.items != 0) {
    for (int64_t i = 0; i < node.children.length; i++) {
      uint64_t child_id = node.children.items[i];
      if (child_id == node.id) {
        ISEARCH_WARNING(stderr, "child_id == node.id\n");
        break;
      }

      ISearchNode *child = isearch_get_node_by_id(s, child_id);
      ISEARCH_ASSERT_RETURN(child != 0, );

      isearch_print_node(s, *child, pad + 2, fp);
    }
  }

  ISEARCH_PAD_PRINT(pad, "}");
  fprintf(fp, "\n");
}

int isearch_print(ISearch *s, FILE *fp) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);
  ISEARCH_ASSERT_RETURN(fp != 0, 0);

  fprintf(fp, "ISearch {\n");
  for (int64_t i = 0; i < s->nodes.length; i++) {
    ISearchNode node = s->nodes.items[i];
    isearch_print_node(s, node, 2, fp);
  }
  fprintf(fp, "\n}\n");

  return 1;
}

static int isearch_query_node(ISearch *s, ISearchQuery query,
                              ISearchStringBuffer *out, ISearchNode node) {

  ISEARCH_ASSERT_RETURN(node.word.value != 0, 0);
  if (mif_lev_fast(query.word, node.word.value) >= query.max_distance)
    return 0;

  if (query.must_contain != 0) {
    if (strstr(node.word.value, query.must_contain) == 0)
      return 0;
  }

  if (query.must_not_contain != 0) {
    if (strstr(node.word.value, query.must_not_contain) != 0)
      return 0;
  }

  isearch_ISearchString_buffer_push(out, node.word);

  return out->length > 0 && out->items != 0;
}

int isearch_search(ISearch *s, ISearchQuery query, ISearchStringBuffer *out) {
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);
  ISEARCH_ASSERT_RETURN(query.word != 0, 0);
  ISEARCH_ASSERT_RETURN(out != 0, 0);
  if (s->nodes.length <= 0 || s->nodes.items == 0)
    return 0;

  out->length = 0;
  out->items = 0;
  out->avail = 0;
  out->capacity = 0;

  ISearchCacheItem *cache = 0;
  char tmp[PATH_MAX];
  memset(&tmp[0], 0, PATH_MAX * sizeof(char));
  if (!isearch_query_to_string(query, tmp, PATH_MAX - 1))
    return 0;

  if ((cache = (ISearchCacheItem *)hashy_map_get(&s->query_cache, tmp))) {
    *out = cache->words;
    return out->length > 0 && out->items != 0;
  }

  if (!out->initialized) {
    isearch_ISearchString_buffer_init(out);
  }

  ArenaRef ref = {0};
  cache = (ISearchCacheItem *)arena_malloc(&s->cache_allocator, &ref);
  ISEARCH_ASSERT_RETURN(cache != 0, 0);
  cache->ref = ref;

  isearch_ISearchString_buffer_init(&cache->words);

  for (int64_t i = 0; i < s->nodes.length; i++) {
    isearch_query_node(s, query, &cache->words, s->nodes.items[i]);
  }

  *out = cache->words;
  hashy_map_set(&s->query_cache, tmp, cache);

  return out->length > 0 && out->items != 0;
}

int isearch_serialize(ISearch s, FILE *fp) {
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  ISEARCH_ASSERT_RETURN(s.initialized == true, 0);
  return isearch_ISearchNode_buffer_serialize(s.nodes, fp);
}
int isearch_deserialize(ISearch *s, FILE *fp) {
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  ISEARCH_ASSERT_RETURN(s != 0, 0);
  ISEARCH_ASSERT_RETURN(s->initialized == true, 0);

  return isearch_ISearchNode_buffer_deserialize(&s->nodes, &s->string_allocator,
                                                fp);
}
