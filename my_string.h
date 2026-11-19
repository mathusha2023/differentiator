#ifndef MY_STRING_H
#define MY_STRING_H

#include <stddef.h>

typedef struct String
{
    char *p;
    size_t length;
} String;

char *copy_string_to_char_arr(String string);
char *stringchr(String string, char c);
char *get_str_num(double num);
int nonregister_strcmp(const char *s1, const char *s2);

#endif // MY_STRING_H