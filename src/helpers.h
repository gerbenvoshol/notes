#ifndef HELPERS_H
#define HELPERS_H 1

#include <stdio.h>
#include <stdbool.h>

void strreplace(char *str, char from, char to);
size_t strmatch(const char *str1, size_t size1, const char *str2, size_t size2);
size_t read_content(FILE *f, char *buffer, size_t size);
int copy_content(FILE *in_f, FILE *out_f);
int last_char(const char *str, FILE *out_f);
void fname(char *buf, const char *str);
void basename(char *buf, const char *str);
bool is_external(const char *url, size_t size);

#endif // !HELPERS_H

