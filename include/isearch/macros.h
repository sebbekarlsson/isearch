#ifndef ISEARCH_MACROS_H
#define ISEARCH_MACROS_H

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a) MAX(-a, a)
#endif
#define ISEARCH_OR(a, b) ((a) ? (a) : (b))

#define ISEARCH_CLI_RED "\x1B[31m"
#define ISEARCH_CLI_GREEN "\x1B[32m"
#define ISEARCH_CLI_YELLLOW "\x1B[33m"
#define ISEARCH_CLI_BLUE "\x1B[34m"
#define ISEARCH_CLI_MAGENTA "\x1B[35m"
#define ISEARCH_CLI_CYAN "\x1B[36m"
#define ISEARCH_CLI_WHITE "\x1B[37m"
#define ISEARCH_CLI_RESET "\x1B[0m"

#define ISEARCH_WARNING(...)                                                  \
  {                                                                       \
    printf(ISEARCH_CLI_RED "(ISEARCH)(Warning)(%s): \n" ISEARCH_CLI_RESET, __func__); \
    fprintf(__VA_ARGS__);                                                 \
  }

#define ISEARCH_WARNING_RETURN(ret, ...)                                      \
  {                                                                       \
    printf("\n****\n");                                                   \
    printf(ISEARCH_CLI_RED "(ISEARCH)(Warning)(%s): \n" ISEARCH_CLI_RESET, __func__); \
    fprintf(__VA_ARGS__);                                                 \
    printf("\n****\n");                                                   \
    return ret;                                                           \
  }

#define ISEARCH_WARNING_RETURN_NOT_INITIALIZED(ret)                           \
  {                                                                       \
    printf("\n****\n");                                                   \
    printf(ISEARCH_CLI_RED "(ISEARCH)(Warning)(%s): \n" ISEARCH_CLI_RESET, __func__); \
    fprintf(stderr, "Not initialized.\n");                                \
    printf("\n****\n");                                                   \
    return ret;                                                           \
  }

#define ISEARCH_PANIC(fp, ...)        \
  {                               \
    ISEARCH_WARNING(fp, __VA_ARGS__); \
    exit(0);                      \
  }

#define ISEARCH_ASSERT_PANIC(expr)                             \
  {                                                        \
    if (!(expr)) {                                         \
      ISEARCH_PANIC(stderr, "Assertion failed: %s.\n", #expr); \
    }                                                      \
  }

#define ISEARCH_ASSERT_RETURN(expr, ret)                                \
  {                                                                 \
    if (!(expr)) {                                                  \
      ISEARCH_WARNING(stderr, "(ISEARCH): Assertion failed: %s.\n", #expr); \
      return ret;                                                   \
    }                                                               \
  }

#define ISEARCH_FREE(v) free(v)
#define ISEARCH_REALLOC(ptr, len) realloc(ptr, len)
#define ISEARCH_CALLOC(n, s) calloc(n, s)

#define ISEARCH_GENERATE_ENUM(ENUM) ENUM,
#define ISEARCH_GENERATE_STRING(STRING) #STRING,

#define ISEARCH_PAD_PRINTF(pad, fmt, ...) printf("%*s" fmt, pad, "", __VA_ARGS__);
#define ISEARCH_PAD_SNPRINTF(buff, cap, pad, fmt, ...) \
  snprintf(buff, cap, "%*s" fmt, pad, "", __VA_ARGS__);
#define ISEARCH_PAD_PRINT(pad, buff) printf("%*s%s", pad, "", buff);

#define ISEARCH_DEFINE_ENUM(enum_name, CAPITAL_NAME, FOR_EACH_MACRO) \
  typedef enum { FOR_EACH_MACRO(ISEARCH_GENERATE_ENUM) } enum_name;  \
  static const char* const CAPITAL_NAME##_TO_STR[] = {FOR_EACH_MACRO(ISEARCH_GENERATE_STRING)};



#define ISEARCH_PRINT_N_E(n, v, end, fp)                                         \
  {                                                                            \
    for (int64_t g = 0; g < (n); g++) {                                        \
      fprintf(fp, "%s", v);                                                    \
    }                                                                          \
    fprintf(fp, end);                                                          \
  }


#define ISEARCH_PRINT_N(n, v, fp) { for (int64_t g = 0; g < (n); g++) { fprintf(fp, "%s", v);  } }

#endif
