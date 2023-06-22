#include <isearch/isearch.h>
#include <isearch/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <isearch/macros.h>
#include <isearch/node.h>

int main(int argc, char *argv[]) {
  ISearch s = {0};

  isearch_init(&s, (ISearchConfig){0});

  isearch_add_word(&s, "volvo");
  isearch_add_word(&s, "mercedes");
  isearch_add_word(&s, "saab");
  isearch_add_word(&s, "toyota");
  isearch_add_word(&s, "hello");
  isearch_add_word(&s, "world");
  isearch_add_word(&s, "duck");
  isearch_add_word(&s, "rabbit");
  isearch_add_word(&s, "dog");
  isearch_add_word(&s, "cat");
  isearch_add_word(&s, "cosine");
  isearch_add_word(&s, "sine");
  isearch_add_word(&s, "tan");
  isearch_add_word(&s, "tanhf");
  isearch_add_word(&s, "add");
  isearch_add_word(&s, "subtract");
  isearch_add_word(&s, "multiply");
  isearch_add_word(&s, "mat2");
  isearch_add_word(&s, "sound.jpg");
  isearch_add_word(&s, "sound.wav");
  isearch_add_word(&s, "mat3");
  isearch_add_word(&s, "mat4");
  isearch_add_word(&s, "vec2");
  isearch_add_word(&s, "vec3");
  isearch_add_word(&s, "vec4");

  isearch_build_association_tree(&s);

  ISearchStringBuffer result = {0};

  if (isearch_search(&s, (ISearchQuery){ .word = "sound", .max_distance = 5, .must_contain = ".wav" }, &result)) {
    isearch_ISearchString_buffer_print(result, stdout);
  }

  isearch_purge_cache(&s);

  if (isearch_search(&s, (ISearchQuery){ .word = "sound", .max_distance = 5, .must_contain = ".wav" }, &result)) {
    isearch_ISearchString_buffer_print(result, stdout);
  }

  FILE* fp = fopen("isearch.bin", "wb+");
  isearch_serialize(s, fp);
  fclose(fp);

  fp = fopen("isearch.bin", "rb");
  ISearch s2 = {0};
  isearch_init(&s2, (ISearchConfig){0});
  if (isearch_deserialize(&s2, fp)) {
    isearch_print(&s2, stdout);
  }
  fclose(fp);

  isearch_destroy(&s);
  isearch_destroy(&s2);

  return 0;
}
