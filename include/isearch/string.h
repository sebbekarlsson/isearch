#ifndef ISEARCH_STRING_H
#define ISEARCH_STRING_H
#include <isearch/buffer.h>
#include <stdint.h>
#include <stdio.h>
#include <bytepager/bytepager.h>
typedef struct {
  uint64_t id;
  uint64_t length;
  const char* value;
} ISearchString;
ISEARCH_DEFINE_BUFFER(ISearchString);

int isearch_ISearchString_buffer_print(ISearchStringBuffer buff, FILE *fp);

int isearch_string_serialize(ISearchString sstring, FILE* fp);
int isearch_string_deserialize(ISearchString* sstring, Bytepager* allocator, FILE* fp);
#endif
