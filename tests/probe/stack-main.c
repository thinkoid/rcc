/* The stack probe's main, compiled by clang -O0: calls every function
   of stack.c as rcc compiled it and as clang did (ref_), over a table
   of values, and compares the results.  Prints the first forty
   differences and a summary; exits 0 when nothing differs, 1
   otherwise. */

#include <limits.h>
#include <stdio.h>

#include "stack.h"

/* clang-format off */
static const long values[] = {
    0, 1, 2, 3, 5, 7, 85, 100, 127, 128, 200, 255, 256, 4095, 4096, 4097,
    5000, 32767, 32768, 65535, 65536, 0x12345, 0xfff000, 0x12345678,
    0x7fffffff, 0x80000000L, 0xffffffffL, 0x100000000L, 0x123456789abcdef0L,
    -1, -2, -3, -7, -128, -129, -4096, -5000, -32768, -32769, -65536,
    -2147483647L - 1, -2147483649L, LONG_MAX, LONG_MIN
};
/* clang-format on */

#define COUNT ((int) (sizeof values / sizeof *values))

static long total;
static long bad;
static long shown = 40;

/* f and its reference on every pair of values */
static void
check(const char *name, long (*f)(long, long), long (*ref)(long, long))
{
        for (int i = 0; i < COUNT; ++i)
                for (int j = 0; j < COUNT; ++j) {
                        const long got = f(values[i], values[j]);
                        const long want = ref(values[i], values[j]);

                        ++total;
                        if (got == want)
                                continue;

                        if (++bad <= shown)
                                printf("%s(%ld, %ld): rcc %ld, clang %ld\n",
                                       name, values[i], values[j], got, want);
                }
}

int
main(void)
{
#define CHECK(name) check(#name, name, ref_##name);
        FUNCTIONS(CHECK)
#undef CHECK

        printf("%ld checks, %ld differ\n", total, bad);

        return bad != 0;
}
