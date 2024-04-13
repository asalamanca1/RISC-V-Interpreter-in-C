#include "custom_atoi.h"

int custom_atoi(const char *str) {
  int result = 0;
  int sign = 1;
  int i = 0;

  // Handle leading whitespace
  while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n') i++;

  // Handle optional sign
  if (str[i] == '-') {
    sign = -1;
    i++;
  } else if (str[i] == '+') {
    i++;
  }

  // Process digits
  while (str[i] >= '0' && str[i] <= '9') {
    result = result * 10 + (str[i] - '0');
    i++;
  }

  return sign * result;
}
