#ifndef ISEARCH_UINT64_H
#define ISEARCH_UINT64_H
#include <isearch/buffer.h>
#include <stdio.h>
ISEARCH_DEFINE_BUFFER(uint64_t);


int isearch_uint64_t_buffer_serialize(uint64_tBuffer buffer, FILE* fp);
int isearch_uint64_t_buffer_deserialize(uint64_tBuffer* buffer, FILE* fp);

#endif
