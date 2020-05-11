#include <stdio.h>
#include <string.h>
#include <errno.h>

/* unless otherwise specified, none of the following
 * functions work on non-ascii characters.
 */

void strreplace(char *str, char from, char to) {
  for (int i = 0; str[i] != 0; i++) {
    if (str[i] == from) {
      str[i] = to;
    }
  }
}

size_t strmatch(const char *str1, size_t size1, const char *str2, size_t size2) {
  size_t match = 0;
  for (size_t i = 0; i < size1 && i < size2; i++) {
    if (str1[i] == str2[i]) {
      match++;
    } else {
      return match;
    }
  }

  return match;
}

size_t read_content(FILE *f, char *buffer, size_t size) {
  for (size_t i = 0; i < size; i++) {
    int c = fgetc(f);
    if (feof(f)) {
      buffer[i] = '\0';
      return i;
    } else if ((errno = ferror(f))) {
      perror("read_content");
      return -1;
    } else {
      buffer[i] = c;
    }
  }

  fprintf(stderr, "read_content: file size bigger than buffer size.\n");
  return size;
}

int copy_content(FILE *in_f, FILE *out_f) {
  char buffer[512];

  for (;;) {
    size_t sz = fread(buffer, 1, sizeof(buffer), in_f);
    if (sz) {
      fwrite(buffer, 1, sz, out_f);
    } else if (feof(in_f)) {
      break;
    } else if ((errno = ferror(in_f))) {
      perror("copy_content");
      return 1;
    }
  }

  return 0;
}

int last_char(const char *str, const char c) {
  for (int i = strlen(str) - 1; i > 0; i--) {
    if (str[i] == c) {
      return i;
    }
  }

  return 0;
}

void fname(char *buf, const char *str) {
  int extension_point = last_char(str, '/');

  for (int i = 0; str[i + extension_point] != 0; i++) {
    buf[i] = str[i + extension_point];
  }
}

void basename(char *buf, const char *str) {
  int extension_point = last_char(str, '.');
  int starting_point = last_char(str, '/') + 1;

  for (int i = 0; i < extension_point - starting_point; i++) {
    buf[i] = str[starting_point + i];
  }
}
