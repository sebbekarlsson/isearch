#include <bytepager/bytepager.h>
#include <isearch/buffer.h>
#include <isearch/macros.h>
#include <isearch/string.h>
#include <stdio.h>
#include <stdlib.h>

ISEARCH_IMPLEMENT_BUFFER(ISearchString);

int isearch_ISearchString_buffer_print(ISearchStringBuffer buff, FILE *fp) {
  if (buff.length <= 0 || buff.items == 0 || fp == 0)
    return 0;

  for (int64_t i = 0; i < buff.length; i++) {
    ISearchString sstring = buff.items[i];
    if (sstring.value == 0)
      continue;
    fprintf(fp, "%s\n", sstring.value);
  }
  return 1;
}

int isearch_string_serialize(ISearchString sstring, FILE *fp) {
  if (!fp)
    return 0;

  ISEARCH_ASSERT_RETURN(sstring.length > 0, 0);
  ISEARCH_ASSERT_RETURN(sstring.value != 0, 0);

  fwrite(&sstring.id, sizeof(sstring.id), 1, fp);
  fwrite(&sstring.length, sizeof(sstring.length), 1, fp);
  fwrite(&sstring.value[0], sizeof(char), sstring.length, fp);

  return 1;
}
int isearch_string_deserialize(ISearchString *sstring, Bytepager *allocator,
                               FILE *fp) {
  ISEARCH_ASSERT_RETURN(sstring != 0, 0);
  ISEARCH_ASSERT_RETURN(allocator != 0, 0);
  ISEARCH_ASSERT_RETURN(allocator->initialized == true, 0);
  fread(&sstring->id, sizeof(sstring->id), 1, fp);
  fread(&sstring->length, sizeof(sstring->length), 1, fp);
  ISEARCH_ASSERT_RETURN(sstring->length > 0, 0);
  char *value = bytepager_malloc(allocator, sstring->length + 1);
  ISEARCH_ASSERT_RETURN(value != 0, 0);
  fread(&value[0], sizeof(char), sstring->length, fp);
  sstring->value = (const char *)value;
  return 1;
}
