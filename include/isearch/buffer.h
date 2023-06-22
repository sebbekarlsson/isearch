#ifndef ISEARCH_TYPE_BUFFER_H
#define ISEARCH_TYPE_BUFFER_H
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define ISEARCH_DEFINE_BUFFER(T)                                                         \
  typedef struct ISEARCH_##T##_BUFFER_STRUCT {                                           \
    T* items;                                                                        \
    volatile int64_t length;                                                         \
    int64_t avail;                                                                   \
    int64_t capacity;                                                                \
    bool fast;                                                                       \
    volatile bool initialized;                                                       \
  } T##Buffer;                                                                       \
  int isearch_##T##_buffer_init(T##Buffer* buffer);                                      \
  T##Buffer isearch_##T##_buffer_init_inline();                                          \
  int isearch_##T##_buffer_init_fast(T##Buffer* buffer, int64_t capacity);               \
  T* isearch_##T##_buffer_push(T##Buffer* buffer, T item);                               \
  int isearch_##T##_buffer_clear(T##Buffer* buffer);                                     \
  int isearch_##T##_buffer_fill(T##Buffer* buffer, T item, int64_t count);               \
  int isearch_##T##_buffer_fill_concat(T##Buffer* buffer, T item, int64_t count);        \
  int isearch_##T##_buffer_copy(T##Buffer src, T##Buffer* dest);                         \
  int isearch_##T##_buffer_concat(T##Buffer* src, T##Buffer other);                      \
  int isearch_##T##_buffer_popi(T##Buffer* buffer, int64_t index, T* out);               \
  int isearch_##T##_buffer_splice_remove(T##Buffer* buffer, int64_t start, int64_t end); \
  bool isearch_##T##_buffer_is_empty(T##Buffer buffer);                                  \
  int isearch_##T##_buffer_back(T##Buffer buffer, T* out);                               \
  int isearch_##T##_buffer_pop(T##Buffer* buffer);

#define ISEARCH_IMPLEMENT_BUFFER(T)                                                                \
  int isearch_##T##_buffer_init(T##Buffer* buffer) {                                               \
    if (!buffer) return 0;                                                                     \
    if (buffer->initialized) return 1;                                                         \
    buffer->initialized = true;                                                                \
    buffer->items = 0;                                                                         \
    buffer->avail = 0;                                                                         \
    buffer->fast = false;                                                                      \
    buffer->length = 0;                                                                        \
    return 1;                                                                                  \
  }                                                                                            \
  T##Buffer isearch_##T##_buffer_init_inline() {                                                   \
    T##Buffer buffer = {0};                                                                    \
    isearch_##T##_buffer_init(&buffer);                                                            \
    return buffer;                                                                             \
  }                                                                                            \
  int isearch_##T##_buffer_concat(T##Buffer* src, T##Buffer other) {                               \
    if (!src) return 0;                                                                        \
    if (!src->initialized) ISEARCH_WARNING_RETURN_NOT_INITIALIZED(0);                              \
    if (other.length <= 0 || other.items == 0)                                                 \
      ISEARCH_WARNING_RETURN(0, stderr, "other is empty.\n");                                      \
    if (!other.initialized) ISEARCH_WARNING_RETURN(0, stderr, "other not initialized.\n");         \
    if (src->fast || other.fast)                                                               \
      ISEARCH_WARNING_RETURN(0, stderr, "Cannot concat fast buffers.\n");                          \
                                                                                               \
    if (src->items == 0 && src->length <= 0) {                                                 \
      return isearch_##T##_buffer_copy(other, src);                                                \
    }                                                                                          \
                                                                                               \
    int64_t start_index = src->length;                                                         \
                                                                                               \
    src->length += other.length;                                                               \
    src->items = (T*)realloc(src->items, src->length * sizeof(T));                             \
                                                                                               \
    if (src->items == 0) ISEARCH_WARNING_RETURN(0, stderr, "Failed to allocate memory.\n");        \
                                                                                               \
    memcpy(&src->items[start_index], &other.items[0], other.length * sizeof(T));               \
                                                                                               \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_buffer_init_fast(T##Buffer* buffer, int64_t capacity) {                        \
    if (!buffer) return 0;                                                                     \
    if (buffer->initialized) return 1;                                                         \
    buffer->initialized = true;                                                                \
    buffer->items = 0;                                                                         \
    buffer->length = 0;                                                                        \
    buffer->capacity = capacity;                                                               \
    buffer->fast = capacity > 0;                                                               \
    return 1;                                                                                  \
  }                                                                                            \
  T* isearch_##T##_buffer_push_fast(T##Buffer* buffer, T item) {                                   \
    if (!buffer) return 0;                                                                     \
    if (!buffer->initialized) ISEARCH_WARNING_RETURN(0, stderr, "Buffer not initialized\n");       \
                                                                                               \
    if (buffer->capacity <= 0) ISEARCH_WARNING_RETURN(0, stderr, "No capacity!\n");                \
                                                                                               \
    if (buffer->avail <= 0) {                                                                  \
      buffer->avail = buffer->capacity;                                                        \
                                                                                               \
      buffer->items                                                                            \
        = (T*)ISEARCH_REALLOC(buffer->items, (buffer->length + buffer->capacity) * sizeof(T));     \
                                                                                               \
      if (!buffer->items) ISEARCH_WARNING_RETURN(0, stderr, "Could not realloc buffer.\n");        \
    }                                                                                          \
                                                                                               \
    T* ptr = &buffer->items[buffer->length];                                                   \
    buffer->items[buffer->length++] = item;                                                    \
    buffer->avail -= 1;                                                                        \
    if (buffer->avail < 0)                                                                     \
      ISEARCH_WARNING(stderr, "buffer->avail < 0, this should never happen.\n");                   \
    return ptr;                                                                                \
  }                                                                                            \
  T* isearch_##T##_buffer_push(T##Buffer* buffer, T item) {                                        \
    if (buffer->fast && buffer->capacity > 0) return isearch_##T##_buffer_push_fast(buffer, item); \
    if (!buffer) return 0;                                                                     \
    if (!buffer->initialized) ISEARCH_WARNING_RETURN(0, stderr, "Buffer not initialized\n");       \
    buffer->items = (T*)realloc(buffer->items, (buffer->length + 1) * sizeof(T));              \
    if (!buffer->items) ISEARCH_WARNING_RETURN(0, stderr, "Could not realloc buffer.\n");          \
    buffer->items[buffer->length] = item;                                                      \
    T* ptr = &buffer->items[buffer->length];                                                   \
    buffer->length++;                                                                          \
    return ptr;                                                                                \
  }                                                                                            \
  int isearch_##T##_buffer_copy(T##Buffer src, T##Buffer* dest) {                                  \
    if (!dest) return 0;                                                                       \
                                                                                               \
    if (src.length <= 0 || src.items == 0) return 0;                                           \
    if (!dest->initialized) ISEARCH_WARNING_RETURN(0, stderr, "destination not initialized\n");    \
                                                                                               \
    if (!src.initialized) ISEARCH_WARNING_RETURN(0, stderr, "source not initialized\n");           \
                                                                                               \
    if (dest->length > 0 || dest->items != 0)                                                  \
      ISEARCH_WARNING_RETURN(0, stderr, "Destination is not empty!\n");                            \
                                                                                               \
    dest->length = src.length;                                                                 \
    dest->items = (T*)calloc(src.length, sizeof(T));                                           \
                                                                                               \
    if (dest->items == 0) ISEARCH_WARNING_RETURN(0, stderr, "Failed to allocate memory.\n");       \
                                                                                               \
    memcpy(&dest->items[0], &src.items[0], src.length * sizeof(T));                            \
                                                                                               \
    return dest->length > 0 && dest->items != 0;                                               \
  }                                                                                            \
  int isearch_##T##_buffer_clear(T##Buffer* buffer) {                                              \
    if (!buffer) return 0;                                                                     \
    if (!buffer->initialized) return 0;                                                        \
    if (buffer->items != 0) {                                                                  \
      ISEARCH_FREE(buffer->items);                                                                 \
      buffer->items = 0;                                                                       \
    }                                                                                          \
    buffer->items = 0;                                                                         \
    buffer->avail = 0;                                                                         \
    buffer->length = 0;                                                                        \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_buffer_fill(T##Buffer* buffer, T item, int64_t count) {                        \
    if (!buffer) return 0;                                                                     \
    if (count <= 0) return 0;                                                                  \
    if (buffer->fast) ISEARCH_WARNING_RETURN(0, stderr, "Cannot fill a fast buffer.\n");           \
    if (!buffer->initialized) ISEARCH_WARNING_RETURN(0, stderr, "Buffer not initialized\n");       \
    if (buffer->items != 0) {                                                                  \
      isearch_##T##_buffer_clear(buffer);                                                          \
    }                                                                                          \
    buffer->items = (T*)calloc(count, sizeof(T));                                              \
    if (buffer->items == 0) ISEARCH_WARNING_RETURN(0, stderr, "Failed to allocate memory.\n");     \
    buffer->length = count;                                                                    \
    for (int64_t i = 0; i < buffer->length; i++) {                                             \
      buffer->items[i] = item;                                                                 \
    }                                                                                          \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_buffer_fill_concat(T##Buffer* buffer, T item, int64_t count) {                 \
    if (!buffer) return 0;                                                                     \
    if (count <= 0) return 0;                                                                  \
    if (buffer->fast) ISEARCH_WARNING_RETURN(0, stderr, "Cannot fill a fast buffer.\n");           \
    if (!buffer->initialized) ISEARCH_WARNING_RETURN(0, stderr, "Buffer not initialized\n");       \
                                                                                               \
    if (buffer->length <= 0) {                                                                 \
      return isearch_##T##_buffer_fill(buffer, item, count);                                       \
    }                                                                                          \
                                                                                               \
    int64_t start_index = buffer->length;                                                      \
                                                                                               \
    buffer->length += count;                                                                   \
    buffer->items = (T*)realloc(buffer->items, buffer->length * sizeof(T));                    \
                                                                                               \
    if (buffer->items == 0) {                                                                  \
      ISEARCH_WARNING_RETURN(0, stderr, "Failed to allocate memory.\n");                           \
    }                                                                                          \
                                                                                               \
    for (int64_t i = start_index; i < buffer->length; i++) {                                   \
      buffer->items[i] = item;                                                                 \
    }                                                                                          \
                                                                                               \
    return buffer->items != 0 && buffer->length > 0;                                           \
  }                                                                                            \
  int isearch_##T##_buffer_popi(T##Buffer* buffer, int64_t index, T* out) {                        \
    if (!buffer) return 0;                                                                     \
    if (!buffer->initialized) ISEARCH_WARNING_RETURN(0, stderr, "Buffer not initialized\n");       \
                                                                                               \
    if (isearch_##T##_buffer_is_empty(*buffer) || index < 0 || index >= buffer->length) return 0;  \
                                                                                               \
    *out = buffer->items[index];                                                               \
                                                                                               \
    if (buffer->length - 1 <= 0) {                                                             \
      isearch_##T##_buffer_clear(buffer);                                                          \
      return 0;                                                                                \
    }                                                                                          \
                                                                                               \
    for (int i = index; i < buffer->length - 1; i++) {                                         \
      buffer->items[i] = buffer->items[i + 1];                                                 \
    }                                                                                          \
                                                                                               \
    buffer->items = (T*)realloc(buffer->items, (buffer->length - 1) * sizeof(T));              \
    buffer->length -= 1;                                                                       \
    buffer->length = MAX(0, buffer->length);                                                   \
                                                                                               \
    return 1;                                                                                  \
  }                                                                                            \
  bool isearch_##T##_buffer_is_empty(T##Buffer buffer) {                                           \
    return (buffer.items == 0 || buffer.length <= 0);                                          \
  }                                                                                            \
  int isearch_##T##_buffer_splice_remove(T##Buffer* buffer, int64_t start, int64_t end) {          \
    if (buffer->length <= 0 || buffer->items == 0) return 0;                                   \
                                                                                               \
    T##Buffer next_buffer = {0};                                                               \
    isearch_##T##_buffer_init(&next_buffer);                                                       \
                                                                                               \
    for (int64_t i = start; i < MIN(start + end, buffer->length - 1); i++) {                   \
      T v = buffer->items[i % buffer->length];                                                 \
      isearch_##T##_buffer_push(&next_buffer, v);                                                  \
    }                                                                                          \
                                                                                               \
    isearch_##T##_buffer_clear(buffer);                                                            \
    *buffer = next_buffer;                                                                     \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_buffer_back(T##Buffer buffer, T* out) {                                        \
    if (buffer.length <= 0 || buffer.items == 0) return 0;                                     \
    *out = buffer.items[buffer.length - 1];                                                    \
    return 1;                                                                                  \
  }                                                                                            \
  int isearch_##T##_buffer_pop(T##Buffer* buffer) {                                                \
    if (buffer->length <= 0 || buffer->items == 0) return 0;                                   \
                                                                                               \
    if (buffer->length - 1 <= 0) {                                                             \
      isearch_##T##_buffer_clear(buffer);                                                          \
      return 1;                                                                                \
    }                                                                                          \
    buffer->items = (T*)realloc(buffer->items, (buffer->length - 1) * sizeof(T));              \
    buffer->length -= 1;                                                                       \
    return 1;                                                                                  \
  }

#endif
