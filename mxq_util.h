
#ifndef __MXQ_UTIL_H__
#define __MXQ_UTIL_H__ 1

#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "mxq.h"

char *mxq_hostname(void);

int log_msg(int prio, const char *fmt, ...) __attribute__ ((format(printf, 2, 3)));

#define print_error(fmt,...) log_msg(1, "%s:%d:%s:" fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

void mxq_free_job(struct mxq_job_full *job);

char **stringtostringvec(int argc, char *s);
char *stringvectostring(int argc, char *argv[]);
int chrcnt(char *s, char c);

int safe_convert_string_to_ull(char *string, unsigned long long int *integer);

int safe_convert_string_to_ui8(char *string, u_int8_t *integer);
int safe_convert_string_to_ui16(char *string, u_int16_t *integer);
int safe_convert_string_to_ui32(char *string, u_int32_t *integer);
int safe_convert_string_to_ui64(char *string, u_int64_t *integer);

void *realloc_or_free(void *ptr, size_t size);
void *realloc_forever(void *ptr, size_t size);

char** strvec_new(void);
size_t strvec_length(char **strvec);
int    strvec_push_str(char ***strvecp, char *str);
int    strvec_push_strvec(char*** strvecp, char **strvec);
char*  strvec_to_str(char **strvec);
char** str_to_strvec(char *str);
void   strvec_free(char **strvec);

#define streq(a, b) (strcmp((a), (b)) == 0)

#define free_null(a) do { free((a)); (a) = NULL; } while(0);

#define _cleanup_(x) __attribute__((cleanup(x)))

static inline void freep(void *p) {
    free(*(void**) p);
}

#define _cleanup_free_ _cleanup_(freep)

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

int mxq_setenv(const char *name, const char *value);
int mxq_setenvf(const char *name, char *fmt, ...);

#endif