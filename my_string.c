#include "my_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include "config.h"

char *copy_string_to_char_arr(String string)
{
    assert(string.p);

    char *p = (char *)calloc(string.length + 1, sizeof(char));
    if (!p)
    {
        return NULL;
    }

    size_t i = 0;
    for (i = 0; i < string.length; i++)
    {
        p[i] = string.p[i];
    }
    p[i] = '\0';

    return p;
}

char *stringchr(String string, char c)
{
    assert(string.p);

    for (size_t i = 0; i < string.length; i++)
    {
        if (string.p[i] == c)
            return string.p + i;
    }
    return NULL;
}

char *get_str_num(double num)
{
    char buff[MAX_NODE_DATA_LENGTH] = {};

    int res = snprintf(buff, MAX_NODE_DATA_LENGTH, "%lg", num);

    if (res < 0)
        return NULL;

    char *p = (char *)calloc((size_t)res + 1, sizeof(char));

    return p;
}

int nonregister_strcmp(const char *s1, const char *s2)
{
    assert(s1);
    assert(s2);

    size_t i = 0;
    for (i = 0; s1[i] && s2[i]; i++)
    {
        if (tolower(s1[i]) != tolower(s2[i]))
            break;
    }
    return tolower(s1[i]) - tolower(s2[i]);
}
