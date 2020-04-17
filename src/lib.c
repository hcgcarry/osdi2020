#include <stdarg.h>
#include "types.h"

char *strcpy(char *dest, const char *src) {
  uint64_t i = 0;
  for (; src[i] != '\0'; ++i) {
    dest[i] = src[i];
  }
  dest[i] = src[i];
  return dest;
}

int printf(const char *format, ...) {
  va_list ap;
  char buf[256];
  va_start(ap, format);

  char *iter = format, *out = buf;
  while (*iter != '\0') {
    if (*iter == '%') {
      while (true) {
        ++iter;
        if (*iter == '#') {
          if (*(iter + 1) == 'x') {
            out += strlen(strcpy(out, "0x"));
          } else {
            --iter;
            break;
          }
        } else if (*iter == 'u') {
          out += strlen(uitos(va_arg(ap, uint64_t), out));
          break;
        } else if (*iter == 'x') {
          out += strlen(uitos_generic(va_arg(ap, uint64_t), 16, out));
          break;
        } else if (*iter == 's') {
          out += strlen(strcpy(out, va_arg(ap, char *)));
          break;
        } else {
          mini_uart_puts("[ERROR] Invalid format specifier\n");
          return -1;
        }
      }
      ++iter;
    } else {
      *out = *iter;
      ++iter;
      ++out;
    }
  }

  *out = '\0';
  mini_uart_puts(buf);
  return strlen(buf);
}
