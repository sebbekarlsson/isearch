#include <isearch/buffer.h>
#include <isearch/macros.h>
#include <isearch/uint64.h>
#include <stdio.h>
#include <stdlib.h>

ISEARCH_IMPLEMENT_BUFFER(uint64_t);

int isearch_uint64_t_buffer_serialize(uint64_tBuffer buffer, FILE *fp) {
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  int64_t len = buffer.initialized == false ? 0 : buffer.length;

  fwrite(&len, sizeof(buffer.length), 1, fp);
  if (len > 0 && buffer.initialized == true) {
    for (int64_t i = 0; i < buffer.length; i++) {
      uint64_t v = buffer.items[i];
      fwrite(&v, sizeof(v), 1, fp);
    }
  }

  return 1;
}
int isearch_uint64_t_buffer_deserialize(uint64_tBuffer *buffer, FILE *fp) {
  ISEARCH_ASSERT_RETURN(buffer != 0, 0);
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  if (!buffer->initialized) {
    isearch_uint64_t_buffer_init(buffer);
  }

  int64_t len = 0;
  fread(&len, sizeof(len), 1, fp);

  for (int64_t i = 0; i < len; i++) {
    uint64_t v = 0;
    fread(&v, sizeof(v), 1, fp);
    isearch_uint64_t_buffer_push(buffer, v);
  }

  return buffer->length > 0 && buffer->items != 0;
}
