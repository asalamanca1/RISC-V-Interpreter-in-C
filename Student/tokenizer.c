#include <stdio.h>

#include "my_string.h"  //str_chr() str_sep()
#include "tokenizer.h"

/* Return the number of tokens in str by using delims for the
 * special character to tokenize. str and delims are expected
 * to be null terminated.
 */
static size_t __n_tokens(const char *str, const char *delims) {
  size_t k_tokens;

  for (k_tokens = 1; *str != '\0'; ++str)
    if (str_chr(delims, *str) != NULL) ++k_tokens;

  return k_tokens;
}

char **tokenize(char *str, const char *delims) {
  char **tokens;
  char **p;

  size_t n_tokens;

  n_tokens = __n_tokens(str, delims);
  tokens = (char **)malloc((n_tokens + 1) * sizeof(char *));
  if (tokens == NULL) return NULL;

  for (p = tokens; n_tokens != 0; n_tokens--) {
    *p++ = str_sep(&str, delims);
  }

  *p = NULL;

  return tokens;
}
