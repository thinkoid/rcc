/* The lists probe: functions in rcc's C that take a list a variadic
   function's va_start built, 7.16, and read it with va_arg, 7.16.1.1:
   count arguments of one type each, every type at every position;
   arguments of the types a string names; and two copies of a list,
   va_copy, 7.16.1.2, read one after the other, each ended by va_end,
   7.16.1.3.  See lists.h. */

#include "lists.h"

/* count arguments of T, each mixed into the result */
#define READ(A, T)                                                      \
    long read_##A(int count, va_list ap)                                \
    {                                                                   \
        long h = 17;                                                    \
                                                                        \
        while (count-- > 0)                                             \
            h = h * 1000003 + (long) va_arg(ap, T);                     \
        return h;                                                       \
    }

TYPES(READ)

/* the arguments of the types the letters of types name, in order */
long
mixed(const char *types, va_list ap)
{
        long h = 29;

        for (; *types != '\0'; ++types) {
                long v = 0;

                if (*types == 'i')
                        v = va_arg(ap, int);
                else if (*types == 'u')
                        v = va_arg(ap, unsigned);
                else if (*types == 'l')
                        v = va_arg(ap, long);
                else if (*types == 'm')
                        v = (long) va_arg(ap, unsigned long);
                else if (*types == 'p')
                        v = (long) va_arg(ap, char *);
                else if (*types == 'c')
                        v = va_arg(ap, char);
                else
                        v = va_arg(ap, short);

                h = h * 1000003 + v;
        }

        return h;
}

/* count ints read through a copy of the list, then the same through
   the list, so that the copy starts where the list stood */
long
copied(int count, va_list ap)
{
        va_list copy;
        long h = 31;
        int n;

        va_copy(copy, ap);
        for (n = count; n > 0; --n)
                h = h * 1000003 + va_arg(copy, int);
        va_end(copy);

        for (n = count; n > 0; --n)
                h = h * 7 + va_arg(ap, int);

        return h;
}
