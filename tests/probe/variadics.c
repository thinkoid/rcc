/* The variadics probe: variadic definitions in rcc's C, whose
   va_start builds the list from the arguments as the caller passed
   them, 7.16.1.4: count arguments of one type after k named
   parameters; the arguments of the types a string names; a list handed
   to vsnprintf, whose doubles the caller passed in vector registers,
   directly and through a function of rcc's that takes it; a list
   copied, started again, and handed to clang's code; and a call of one
   of them from rcc's code.  See variadics.h. */

#include <stdio.h>

#include "variadics.h"

/* after k named parameters, count arguments of T, each mixed into the
   result */
#define SUM(K, A, T)                                                    \
    long sum##K##_##A(NAMED_##K int count, ...)                         \
    {                                                                   \
        va_list ap;                                                     \
        long h = 17;                                                    \
                                                                        \
        va_start(ap, count);                                            \
        while (count-- > 0)                                             \
            h = h * 1000003 + (long) va_arg(ap, T);                     \
        va_end(ap);                                                     \
        return h;                                                       \
    }

#define SUM_K(K, A) SUM(K, A, TYPE_##A)
#define SUMS(A, T) COUNTS(SUM_K, A)

#define TYPE_i int
#define TYPE_u unsigned
#define TYPE_l long
#define TYPE_m unsigned long
#define TYPE_p char *
#define TYPE_c char
#define TYPE_s short

TYPES(SUMS)

/* the next argument of a list at the type a letter names */
static long
next(char type, va_list *ap)
{
        if (type == 'i')
                return va_arg(*ap, int);
        if (type == 'u')
                return va_arg(*ap, unsigned);
        if (type == 'l')
                return va_arg(*ap, long);
        if (type == 'm')
                return (long) va_arg(*ap, unsigned long);
        if (type == 'p')
                return (long) va_arg(*ap, char *);
        if (type == 'c')
                return va_arg(*ap, char);

        return va_arg(*ap, short);
}

/* after k named parameters, the arguments of the types named */
#define MIXED(K, A)                                                     \
    long mixed##K(NAMED_##K const char *types, ...)                     \
    {                                                                   \
        va_list ap;                                                     \
        long h = 29;                                                    \
                                                                        \
        va_start(ap, types);                                            \
        for (; *types != '\0'; ++types)                                 \
            h = h * 1000003 + next(*types, &ap);                        \
        va_end(ap);                                                     \
        return h;                                                       \
    }

COUNTS(MIXED, )

/* the arguments formatted by the C library, the list handed on */
int
format(char *buffer, size_t size, const char *form, ...)
{
        va_list ap;
        int n;

        va_start(ap, form);
        n = vsnprintf(buffer, size, form, ap);
        va_end(ap);

        return n;
}

/* the same through a function that takes the list */
int
vformat(char *buffer, size_t size, const char *form, va_list ap)
{
        return vsnprintf(buffer, size, form, ap);
}

int
format_through(char *buffer, size_t size, const char *form, ...)
{
        va_list ap;
        int n;

        va_start(ap, form);
        n = vformat(buffer, size, form, ap);
        va_end(ap);

        return n;
}

/* count ints read through a copy of the list, then through the list */
long
copied(int count, ...)
{
        va_list ap, copy;
        long h = 31;
        int n;

        va_start(ap, count);
        va_copy(copy, ap);
        for (n = count; n > 0; --n)
                h = h * 1000003 + va_arg(copy, int);
        va_end(copy);

        for (n = count; n > 0; --n)
                h = h * 7 + va_arg(ap, int);
        va_end(ap);

        return h;
}

/* count ints read, the list ended and started again, read again */
long
restarted(int count, ...)
{
        va_list ap;
        long h = 37;
        int n;

        va_start(ap, count);
        for (n = count; n > 0; --n)
                h = h * 1000003 + va_arg(ap, int);
        va_end(ap);

        va_start(ap, count);
        for (n = count; n > 0; --n)
                h = h * 7 + va_arg(ap, int);
        va_end(ap);

        return h;
}

/* the list read by clang's code, variadics-lib.c */
long list_sum(int, va_list);

long
relayed(int count, ...)
{
        va_list ap;
        long h;

        va_start(ap, count);
        h = list_sum(count, ap);
        va_end(ap);

        return h;
}

/* a variadic call of rcc's code, from rcc's code */
long
called(void)
{
        return sum3_l(1, 2, 3, 4, -5L, 6L, 0x123456789L, 8L);
}
