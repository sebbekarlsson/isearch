#ifndef ISEARCH_NODE_H
#define ISEARCH_NODE_H
#include <stdint.h>
#include <isearch/string.h>
#include <isearch/uint64.h>
#include <isearch/buffer.h>
#include <bytepager/bytepager.h>
#include <stdio.h>
typedef struct {
  uint64_t id;
  ISearchString word;
  uint64_tBuffer children;
} ISearchNode;

ISEARCH_DEFINE_BUFFER(ISearchNode);

int isearch_node_add_child(ISearchNode *node, uint64_t id);

int isearch_node_destroy(ISearchNode *node);

int isearch_node_serialize(ISearchNode node, FILE* fp);
int isearch_node_deserialize(ISearchNode* node, Bytepager *allocator, FILE *fp);

int isearch_ISearchNode_buffer_serialize(ISearchNodeBuffer buff, FILE* fp);
int isearch_ISearchNode_buffer_deserialize(ISearchNodeBuffer* buff, Bytepager* allocator, FILE* fp);

#endif
