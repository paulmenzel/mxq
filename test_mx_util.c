
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "mx_util.h"

static void test_mx_strskipwhitespaces(void)
{
   char *s;

   assert(s = mx_strskipwhitespaces("     abc   "));
   assert(strcmp(s, "abc   ") == 0);

   assert(s = mx_strskipwhitespaces("abc   "));
   assert(strcmp(s, "abc   ") == 0);

   assert(s = mx_strskipwhitespaces(""));
   assert(strcmp(s, "") == 0);
}

static void test_mx_strtoul(void)
{
    unsigned long int l;

    assert(mx_strtoul("123", &l) == 0);
    assert(l == 123);

    assert(mx_strtoul("  123  ", &l) == 0);
    assert(l == 123);

    assert(mx_strtoul("0173", &l) == 0);
    assert(l == 123);

    assert(mx_strtoul("0x007b", &l) == 0);
    assert(l == 123);

    assert(mx_strtoul("0x007B", &l) == 0);
    assert(l == 123);

    assert(mx_strtoul("+123", &l) == 0);
    assert(l == 123);

    assert(mx_strtoul("-1", &l) == -ERANGE);
    assert(mx_strtoul(" -1", &l) == -ERANGE);

    assert(mx_strtoul("123 123", &l) == -EINVAL);
    assert(mx_strtoul("123s", &l) == -EINVAL);
    assert(mx_strtoul("0888", &l) == -EINVAL);
    assert(mx_strtoul("1.2", &l)  == -EINVAL);
    assert(mx_strtoul("1,2", &l)  == -EINVAL);
    assert(mx_strtoul("test", &l) == -EINVAL);
}

static void test_mx_strtoull(void)
{
    unsigned long long int l;

    assert(mx_strtoull("123", &l) == 0);
    assert(l == 123);

    assert(mx_strtoull("0173", &l) == 0);
    assert(l == 123);

    assert(mx_strtoull("0x007b", &l) == 0);
    assert(l == 123);

    assert(mx_strtoull("0x00000000000000000000000000000007b", &l) == 0);
    assert(l == 123);

    assert(mx_strtoull("0x007B", &l) == 0);
    assert(l == 123);

    assert(mx_strtoull("-127", &l) == -ERANGE);
    assert(mx_strtoull(" -128", &l) == -ERANGE);

    assert(mx_strtoull("0888", &l) == -EINVAL);
    assert(mx_strtoull("1.2", &l) == -EINVAL);
    assert(mx_strtoull("1,2", &l) == -EINVAL);
    assert(mx_strtoull("test", &l) == -EINVAL);
}

static void test_mx_strtou8(void)
{
    uint8_t u;

    assert(mx_strtou8("255", &u) == 0);
    assert(u == 255);

    assert(mx_strtou8("256", &u) == -ERANGE);
}

static void test_mx_strtoi8(void)
{
    int8_t u;

    assert(mx_strtoi8("127", &u) == 0);
    assert(u == 127);

    assert(mx_strtoi8("-127", &u) == 0);
    assert(u == -127);

    assert(mx_strtoi8(" -128", &u) == 0);
    assert(u == -128);

    assert(mx_strtoi8("128", &u) == -ERANGE);
    assert(mx_strtoi8("-129", &u) == -ERANGE);
}

static void test_mx_strbeginswith(void)
{
    char *end = NULL;

    assert(mx_strbeginswith("blahblubb", "", NULL));

    assert(mx_strbeginswith("blahblubb", "", &end));
    assert(strcmp(end, "blahblubb") == 0);

    assert(mx_strbeginswith("BlahBlubb", "Blah", &end));
    assert(strcmp(end, "Blubb") == 0);

    assert(mx_strbeginswith("blahblubb", "blahblubb", &end));
    assert(*end == 0);

    end = NULL;

    assert(mx_strbeginswith("blahblubb", "blubb", &end) == 0);
    assert(end == NULL);

    assert(mx_strbeginswith("blah", "blahblubb", &end) == 0);
    assert(end == NULL);

    assert(mx_strbeginswith("Blahblubb", "blah", &end) == 0);
    assert(end == NULL);

    assert(mx_strbeginswith("", "blah", &end) == 0);
    assert(end == NULL);
}

static void test_mx_stribeginswith(void)
{
    char *end = NULL;

    assert(mx_stribeginswith("blahblubb", "", NULL));

    assert(mx_stribeginswith("blahblubb", "", &end));
    assert(strcmp(end, "blahblubb") == 0);

    assert(mx_stribeginswith("BlahBlubb", "Blah", &end));
    assert(strcmp(end, "Blubb") == 0);

    assert(mx_stribeginswith("BlahBlubb", "bLaH", &end));
    assert(strcmp(end, "Blubb") == 0);


    assert(mx_stribeginswith("blahblubb", "BlahBluBB", &end));
    assert(*end == 0);

    end = NULL;

    assert(mx_stribeginswith("blahblubb", "blubb", &end) == 0);
    assert(end == NULL);

    assert(mx_stribeginswith("blah", "blahblubb", &end) == 0);
    assert(end == NULL);

    assert(mx_stribeginswith("", "blah", &end) == 0);
    assert(end == NULL);
}

static void test_mx_strbeginswithany(void)
{
    char *end = NULL;

    assert(mx_strbeginswithany("blahblubb", (char *[]){ "bla", "blah", NULL }, &end));
    assert(strcmp(end, "blubb") == 0);

    assert(mx_strbeginswithany("blablubb", (char *[]){ "bla", "blah", NULL }, &end));
    assert(strcmp(end, "blubb") == 0);

    end = NULL;
    assert(mx_strbeginswithany("blubb", (char *[]){ "bla", "blah", NULL }, &end) == 0);
    assert(end == NULL);
}

static void test_mx_strtoseconds(void)
{
    unsigned long long int l;

    assert(mx_strtoseconds("123s", &l) == 0);
    assert(l == 123);

    assert(mx_strtoseconds("0123s", &l) == 0);
    assert(l == 123);

    assert(mx_strtoseconds("123s0s", &l) == 0);
    assert(l == 123);

    assert(mx_strtoseconds("2m3s", &l) == 0);
    assert(l == 123);

    assert(mx_strtoseconds(" 2 m 3 s ", &l) == 0);
    assert(l == 123);

    assert(mx_strtoseconds("1h 2m 3s", &l) == 0);
    assert(l == 60*60 + 2*60 + 3);

    assert(mx_strtoseconds("2m 3s 1h", &l) == 0);
    assert(l == 60*60 + 2*60 + 3);

    assert(mx_strtoseconds("2m 3s 1h1y", &l) == 0);
    assert(l == 60*60 + 2*60 + 3 + 52*7*24*60*60);

    assert(mx_strtoseconds("2m 3s 1h1y", &l) == 0);
    assert(l == 60*60 + 2*60 + 3 + 52*7*24*60*60);

    assert(mx_strtoseconds("-1", &l) == -ERANGE);
    assert(mx_strtoseconds(" -1", &l) == -ERANGE);

    assert(mx_strtoseconds("123", &l)  == -EINVAL);
    assert(mx_strtoseconds("0123", &l)  == -EINVAL);
    assert(mx_strtoseconds("1.2", &l)  == -EINVAL);
    assert(mx_strtoseconds("1,2", &l)  == -EINVAL);
    assert(mx_strtoseconds("test", &l) == -EINVAL);
}

static void test_mx_strtominutes(void)
{
    unsigned long long int l;

    assert(mx_strtominutes("123s", &l) == 0);
    assert(l == 2);

    assert(mx_strtominutes("20d", &l) == 0);
    assert(l == 20*24*60);


    assert(mx_strtominutes("-1", &l) == -ERANGE);
    assert(mx_strtominutes(" -1", &l) == -ERANGE);

    assert(mx_strtominutes("123", &l)  == -EINVAL);
    assert(mx_strtominutes("0123", &l)  == -EINVAL);
    assert(mx_strtominutes("1.2", &l)  == -EINVAL);
    assert(mx_strtominutes("1,2", &l)  == -EINVAL);
    assert(mx_strtominutes("test", &l) == -EINVAL);
}

static void test_mx_strtobytes(void)
{
    unsigned long long int l;

    assert(mx_strtobytes("123B", &l) == 0);
    assert(l == 123);

    assert(mx_strtobytes("2M", &l) == 0);
    assert(l == 2*1024*1024);

    assert(mx_strtobytes("1M1024k", &l) == 0);
    assert(l == 2*1024*1024);

    assert(mx_strtobytes("1024k1024K", &l) == 0);
    assert(l == 2*1024*1024);

    assert(mx_strtobytes("-1", &l) == -ERANGE);
    assert(mx_strtobytes(" -1", &l) == -ERANGE);

    assert(mx_strtobytes("2.5M", &l)  == -EINVAL);
    assert(mx_strtobytes("123", &l)  == -EINVAL);
    assert(mx_strtobytes("0123", &l)  == -EINVAL);
    assert(mx_strtobytes("1.2", &l)  == -EINVAL);
    assert(mx_strtobytes("1,2", &l)  == -EINVAL);
    assert(mx_strtobytes("test", &l) == -EINVAL);
}

static void test_mx_read_first_line_from_file(void)
{
    char *str;

    assert(mx_read_first_line_from_file("/proc/sys/kernel/random/boot_id", &str) == 36);
    assert(str);
    mx_free_null(str);

    assert(mx_read_first_line_from_file("/proc/sys/kernel/random/uuid", &str) == 36);
    assert(str);
    mx_free_null(str);

    assert(mx_read_first_line_from_file("/proc/no_such_file", &str) == -ENOENT);
    assert(str == NULL);

    assert(mx_read_first_line_from_file("/proc/self/stat", &str) > 0);
    assert(str);
    mx_free_null(str);
}

static void test_mx_strscan(void)
{
    _mx_cleanup_free_ char *s = NULL;
    char *str;
    unsigned long long int ull;
    long long int ll;

    assert(s = strdup("123 456 -789 246 abc"));
    str = s;

    assert(mx_strscan_ull(&str, &ull) == 0);
    assert(ull == 123);

    assert(mx_strscan_ull(&str, &ull) == 0);
    assert(ull == 456);

    assert(mx_strscan_ull(&str, &ull) == -ERANGE);
    assert(mx_streq(str, "-789 246 abc"));

    assert(mx_strscan_ll(&str, &ll) == 0);
    assert(ll == -789);
    assert(mx_streq(str, "246 abc"));

    assert(mx_strscan_ll(&str, &ll) == 0);
    assert(ll == 246);
    assert(mx_streq(str, "abc"));

    assert(mx_strscan_ull(&str, &ull) == -EINVAL);
    assert(mx_streq(str, "abc"));
    assert(mx_streq(s, "123 456 -789 246 abc"));
    mx_free_null(s);

    assert(s = strdup("123"));
    str = s;
    assert(mx_strscan_ull(&str, &ull) == 0);
    assert(ull == 123);
    assert(mx_streq(str, ""));
    assert(mx_streq(s, "123"));

}

int main(int argc, char *argv[])
{
    test_mx_strskipwhitespaces();
    test_mx_strtoul();
    test_mx_strtoull();
    test_mx_strtou8();
    test_mx_strtoi8();
    test_mx_strbeginswith();
    test_mx_stribeginswith();
    test_mx_strbeginswithany();
    test_mx_strtoseconds();
    test_mx_strtominutes();
    test_mx_strtobytes();
    test_mx_read_first_line_from_file();
    test_mx_strscan();
    return 0;
}
