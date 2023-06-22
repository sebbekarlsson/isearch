#include <bytepager/bytepager.h>
#include <isearch/buffer.h>
#include <isearch/macros.h>
#include <isearch/node.h>
#include <isearch/string.h>
#include <isearch/uint64.h>
#include <stdio.h>
#include <stdlib.h>

ISEARCH_IMPLEMENT_BUFFER(ISearchNode);

int isearch_node_add_child(ISearchNode *node, uint64_t id) {
  ISEARCH_ASSERT_RETURN(node != 0, 0);
  ISEARCH_ASSERT_RETURN(id != node->id, 0);

  if (!node->children.initialized) {
    isearch_uint64_t_buffer_init(&node->children);
  }

  isearch_uint64_t_buffer_push(&node->children, id);

  return node->children.length > 0 && node->children.items != 0;
}

int isearch_node_destroy(ISearchNode *node) {
  ISEARCH_ASSERT_RETURN(node != 0, 0);
  isearch_uint64_t_buffer_clear(&node->children);
  node->id = 0;
  node->word = (ISearchString){0};
  return 1;
}

int isearch_node_serialize(ISearchNode node, FILE *fp) {
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  fwrite(&node.id, sizeof(node.id), 1, fp);
  if (!isearch_string_serialize(node.word, fp))
    return 0;

  ISEARCH_ASSERT_RETURN(
      isearch_uint64_t_buffer_serialize(node.children, fp) != 0, 0);

  return 1;
}
int isearch_node_deserialize(ISearchNode *node, Bytepager *allocator,
                             FILE *fp) {
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  ISEARCH_ASSERT_RETURN(node != 0, 0);
  fread(&node->id, sizeof(node->id), 1, fp);
  isearch_string_deserialize(&node->word, allocator, fp);
  isearch_uint64_t_buffer_deserialize(&node->children, fp);
  return 1;
}

int isearch_ISearchNode_buffer_serialize(ISearchNodeBuffer buffer, FILE *fp) {
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  ISEARCH_ASSERT_RETURN(buffer.initialized == true, 0);
  int64_t len = buffer.length;
  fwrite(&len, sizeof(buffer.length), 1, fp);
  for (int64_t i = 0; i < buffer.length; i++) {
    ISearchNode node = buffer.items[i];
    ISEARCH_ASSERT_RETURN(isearch_node_serialize(node, fp) != 0, 0);
  }

  return 1;
}
int isearch_ISearchNode_buffer_deserialize(ISearchNodeBuffer *buffer,
                                           Bytepager *allocator, FILE *fp) {
  ISEARCH_ASSERT_RETURN(buffer != 0, 0);
  ISEARCH_ASSERT_RETURN(fp != 0, 0);
  if (!buffer->initialized) {
    isearch_ISearchNode_buffer_init(buffer);
  }

  int64_t len = 0;
  fread(&len, sizeof(len), 1, fp);

  for (int64_t i = 0; i < len; i++) {
    ISearchNode node = {0};
    ISEARCH_ASSERT_RETURN(isearch_node_deserialize(&node, allocator, fp) != 0,
                          0);
    isearch_ISearchNode_buffer_push(buffer, node);
  }

  return buffer->length > 0 && buffer->items != 0;
}
