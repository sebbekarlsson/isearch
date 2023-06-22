#ifndef ISEARCH_LIST_H
#define ISEARCH_LIST_H
#include <stdbool.h>
#include <stdint.h>
#include <isearch/macros.h>

#define ISEARCH_DEFINE_LIST(T)                               \
  typedef struct ISEARCH_##T##_LIST_STRUCT {                 \
    T** items;                                           \
    volatile int64_t length;                             \
    volatile bool initialized;                           \
  } T##List;                                             \
  int isearch_##T##_list_init(T##List* list);                \
  int isearch_##T##_list_make_unique(T##List* list);         \
  int isearch_##T##_list_reverse(T##List* list);             \
  T* isearch_##T##_list_push(T##List* list, T* item);        \
  T* isearch_##T##_list_push_unique(T##List* list, T* item); \
  T* isearch_##T##_list_popi(T##List* list, int64_t index);  \
  int isearch_##T##_list_clear(T##List* list);               \
  T* isearch_##T##_list_remove(T##List* list, T* item);      \
  bool isearch_##T##_list_includes(T##List list, T* item);   \
  int64_t isearch_##T##_list_count(T##List list, T* item);   \
  int isearch_##T##_list_concat(T##List* a, T##List b);      \
  bool isearch_##T##_list_is_empty(T##List list);

#define ISEARCH_IMPLEMENT_LIST(T)                                                                  \
  int isearch_##T##_list_init(T##List* list) {                                                     \
    if (!list) return 0;                                                                       \
    if (list->initialized) return 1;                                                           \
    list->initialized = true;                                                                  \
    list->items = 0;                                                                           \
    list->length = 0;                                                                          \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_list_make_unique(T##List* list) {                                              \
    if (isearch_##T##_list_is_empty(*list)) return 1;                                              \
    for (int64_t i = 0; i < list->length; i++) {                                               \
      if (isearch_##T##_list_count(*list, list->items[i]) > 1) {                                   \
        isearch_##T##_list_remove(list, list->items[i]);                                           \
      }                                                                                        \
    }                                                                                          \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_list_reverse(T##List* list) {                                                  \
    if (!list->initialized) ISEARCH_WARNING_RETURN(0, stderr, "List not initialized\n");           \
    if (isearch_##T##_list_is_empty(*list)) return 1;                                              \
    for (int64_t low = 0, high = list->length - 1; low < high; low++, high--) {                \
      T* temp = list->items[low];                                                              \
      list->items[low] = list->items[high];                                                    \
      list->items[high] = temp;                                                                \
    }                                                                                          \
    return 1;                                                                                  \
  }                                                                                            \
  int64_t isearch_##T##_list_count(T##List list, T* item) {                                        \
    if (isearch_##T##_list_is_empty(list)) return 0;                                               \
    int64_t count = 0;                                                                         \
    for (int64_t i = 0; i < list.length; i++) {                                                \
      if (list.items[i] == item) count++;                                                      \
    }                                                                                          \
    return count;                                                                              \
  }                                                                                            \
  bool isearch_##T##_list_includes(T##List list, T* item) {                                        \
    if (isearch_##T##_list_is_empty(list)) return false;                                           \
    for (int64_t i = 0; i < list.length; i++) {                                                \
      if (list.items[i] == item) return true;                                                  \
    }                                                                                          \
    return false;                                                                              \
  }                                                                                            \
  T* isearch_##T##_list_remove(T##List* list, T* item) {                                           \
    if (!list) return item;                                                                    \
    if (!list->initialized) ISEARCH_WARNING_RETURN(item, stderr, "List not initialized\n");        \
                                                                                               \
    if (isearch_##T##_list_is_empty(*list)) return item;                                           \
                                                                                               \
    int64_t index = -1;                                                                        \
    for (int64_t i = 0; i < list->length; i++) {                                               \
      if (list->items[i] == item) {                                                            \
        index = i;                                                                             \
        break;                                                                                 \
      }                                                                                        \
    }                                                                                          \
    if (index <= -1) return item;                                                              \
                                                                                               \
    if (list->length - 1 <= 0) {                                                               \
      isearch_##T##_list_clear(list);                                                              \
      return item;                                                                             \
    }                                                                                          \
    for (int i = index; i < list->length - 1; i++) {                                           \
      list->items[i] = list->items[i + 1];                                                     \
    }                                                                                          \
                                                                                               \
    list->items = (T**)ISEARCH_REALLOC(list->items, (list->length - 1) * sizeof(T));               \
    list->length -= 1;                                                                         \
    list->length = MAX(0, list->length);                                                       \
                                                                                               \
    return item;                                                                               \
  }                                                                                            \
  T* isearch_##T##_list_popi(T##List* list, int64_t index) {                                       \
    if (!list) return 0;                                                                       \
    if (!list->initialized) ISEARCH_WARNING_RETURN(0, stderr, "List not initialized\n");           \
                                                                                               \
    if (list->items == 0 || list->length <= 0 || index < 0 || index >= list->length) return 0; \
                                                                                               \
    T* out = list->items[index];                                                               \
                                                                                               \
    if (list->length - 1 <= 0) {                                                               \
      isearch_##T##_list_clear(list);                                                              \
      return out;                                                                              \
    }                                                                                          \
                                                                                               \
    for (int i = index; i < MAX(0, list->length - 1); i++) {                                   \
      list->items[i] = list->items[i + 1];                                                     \
    }                                                                                          \
    int64_t next_len = (list->length - 1);                                                     \
    if (next_len <= 0) {                                                                       \
      ISEARCH_WARNING_RETURN(0, stderr, "List is corrupt!\n");                                     \
    }                                                                                          \
    list->items = (T**)ISEARCH_REALLOC(list->items, next_len * sizeof(T));                         \
    list->length -= 1;                                                                         \
    list->length = MAX(0, list->length);                                                       \
                                                                                               \
    return out;                                                                                \
  }                                                                                            \
  T* isearch_##T##_list_push_unique(T##List* list, T* item) {                                      \
    if (isearch_##T##_list_includes(*list, item)) return item;                                     \
    return isearch_##T##_list_push(list, item);                                                    \
  }                                                                                            \
  T* isearch_##T##_list_push(T##List* list, T* item) {                                             \
    if (!list) return 0;                                                                       \
    if (!list->initialized) ISEARCH_WARNING_RETURN(0, stderr, "List not initialized\n");           \
    if (!item) return 0;                                                                       \
    list->items = (T**)ISEARCH_REALLOC(list->items, (list->length + 1) * sizeof(T*));              \
    if (!list->items) ISEARCH_WARNING_RETURN(0, stderr, "Could not realloc list.\n");              \
    list->items[list->length++] = item;                                                        \
    return item;                                                                               \
  }                                                                                            \
  int isearch_##T##_list_concat(T##List* a, T##List b) {                                           \
    if (!a) return 0;                                                                          \
    if (b.length <= 0 || b.items == 0) return 0;                                               \
    for (int64_t i = 0; i < b.length; i++) {                                                   \
      if (!isearch_##T##_list_includes(*a, b.items[i])) {                                          \
        isearch_##T##_list_push(a, b.items[i]);                                                    \
      }                                                                                        \
    }                                                                                          \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_list_clear(T##List* list) {                                                    \
    if (!list) return 0;                                                                       \
    if (list->items != 0) {                                                                    \
      ISEARCH_FREE(list->items);                                                                   \
    }                                                                                          \
    list->items = 0;                                                                           \
    list->length = 0;                                                                          \
    return 1;                                                                                  \
  }                                                                                            \
  bool isearch_##T##_list_is_empty(T##List list) { return (list.items == 0 || list.length <= 0); }

#endif
