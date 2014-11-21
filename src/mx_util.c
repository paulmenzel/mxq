
#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mx_util.h"

/* wrapper unsigned */

inline int mx_strtoul(char *str, unsigned long int *to)
{
    unsigned long int ul;
    char *end;

    errno = 0;

    ul = strtoul(str, &end, 0);

    if (errno)
        return -errno;

    for (;*end && *end == ' '; end++)
        /* empty */;

    if (!end || str == end || *end) {
        errno = EINVAL;
        return -EINVAL;
    }

    if (strchr(str, '-')) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = ul;

    return 0;
}

inline int mx_strtoull(char *str, unsigned long long int *to)
{
    unsigned long long int ull;
    char *end;

    errno = 0;

    ull = strtoull(str, &end, 0);

    if (errno)
        return -errno;

    for (;*end && *end == ' '; end++)
        /* empty */;

    if (!end || str == end || *end) {
        errno = EINVAL;
        return -EINVAL;
    }

    if (strchr(str, '-')) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = ull;

    return 0;
}

/* wrapper signed */

inline int mx_strtol(char *str, signed long int *to)
{
    long int l;
    char *end;

    errno = 0;

    l = strtoul(str, &end, 0);

    if (errno)
        return -errno;

    for (;*end && *end == ' '; end++)
        /* empty */;

    if (!end || str == end || *end) {
        errno = EINVAL;
        return -EINVAL;
    }

    *to = l;

    return 0;
}

inline int mx_strtoll(char *str, signed long long int *to)
{
    long long int ll;
    char *end;

    errno = 0;

    ll = strtoll(str, &end, 0);

    if (errno)
        return -errno;

    for (;*end && *end == ' '; end++)
        /* empty */;

    if (!end || str == end || *end) {
        errno = EINVAL;
        return -EINVAL;
    }

    *to = ll;

    return 0;
}

/* unsigned */

int mx_strtoui(char *str, unsigned int *to)
{
    unsigned long int ul;
    char *end;
    int res;

    res = mx_strtoul(str, &ul);
    if (res < 0)
        return res;

    if ((unsigned long int)(unsigned int)ul != ul) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (unsigned int)ul;

    return 0;
}

int mx_strtou8(char *str, uint8_t *to)
{
    unsigned long int ul;
    char *end;
    int res;

    res = mx_strtoul(str, &ul);
    if (res < 0)
        return res;

    if ((unsigned long int)(uint8_t)ul != ul) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (uint8_t)ul;

    return 0;
}

int mx_strtou16(char *str, uint16_t *to)
{
    unsigned long int ul;
    char *end;
    int res;

    res = mx_strtoul(str, &ul);
    if (res < 0)
        return res;

    if ((unsigned long int)(uint16_t)ul != ul) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (uint16_t)ul;

    return 0;
}

int mx_strtou32(char *str, uint32_t *to)
{
    unsigned long int ul;
    char *end;
    int res;

    res = mx_strtoul(str, &ul);
    if (res < 0)
        return res;

    if ((unsigned long int)(uint32_t)ul != ul) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (uint32_t)ul;

    return 0;
}

int mx_strtou64(char *str, uint64_t *to)
{
    unsigned long long int ull;
    char *end;
    int res;

    res = mx_strtoull(str, &ull);
    if (res < 0)
        return res;

    if ((unsigned long long int)(uint64_t)ull != ull) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (uint64_t)ull;

    return 0;
}

/* signed */

int mx_strtoi(char *str, signed int *to)
{
    signed long int l;
    char *end;
    int res;

    res = mx_strtol(str, &l);
    if (res < 0)
        return res;

    if ((signed long int)(signed int)l != l) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (signed int)l;

    return 0;
}

int mx_strtoi8(char *str, int8_t *to)
{
    signed long int l;
    char *end;
    int res;

    res = mx_strtol(str, &l);
    if (res < 0)
        return res;

    if ((signed long int)(int8_t)l != l) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (uint8_t)l;

    return 0;
}

int mx_strtoi16(char *str, int16_t *to)
{
    signed long int l;
    char *end;
    int res;

    res = mx_strtol(str, &l);
    if (res < 0)
        return res;

    if ((signed long int)(int16_t)l != l) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (uint16_t)l;

    return 0;
}

int mx_strtoi32(char *str, int32_t *to)
{
    signed long int l;
    char *end;
    int res;

    res = mx_strtol(str, &l);
    if (res < 0)
        return res;

    if ((signed long int)(int32_t)l != l) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (int32_t)l;

    return 0;
}

int mx_strtoi64(char *str, int64_t *to)
{
    signed long long int ll;
    char *end;
    int res;

    res = mx_strtoll(str, &ll);
    if (res < 0)
        return res;

    if ((signed long long int)(int64_t)ll != ll) {
        errno = ERANGE;
        return -ERANGE;
    }

    *to = (int64_t)ll;

    return 0;
}

