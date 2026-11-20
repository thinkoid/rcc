/* The lists probe's library, compiled by clang alone: variadic
   functions whose va_start builds a list, handed to a function of
   lists.c.  The object is linked twice, as it is for rcc's build of
   lists.c and under the prefix ref_ for clang's, so each list reaches
   the build its caller names.  See lists.h. */

#include "lists.h"

#define READ(A, T) long read_##A(int, va_list);

TYPES(READ)

long mixed(const char *, va_list);
long copied(int, va_list);
long stepped(int, va_list);
long fetched(int, va_list);

/* after k named parameters, count arguments of T read by read_A */
#define WRAP(K, A)                                   \
        long wrap##K##_##A(NAMED_##K int count, ...) \
        {                                            \
                va_list ap;                          \
                long r;                              \
                                                     \
                va_start(ap, count);                 \
                r = read_##A(count, ap);             \
                va_end(ap);                          \
                return r;                            \
        }

#define WRAPS(A, T) COUNTS(WRAP, A)

TYPES(WRAPS)

/* after k named parameters, the arguments of the types named */
#define WRAP_MIXED(K, A)                                       \
        long wrap##K##_mixed(NAMED_##K const char *types, ...) \
        {                                                      \
                va_list ap;                                    \
                long r;                                        \
                                                               \
                va_start(ap, types);                           \
                r = mixed(types, ap);                          \
                va_end(ap);                                    \
                return r;                                      \
        }

COUNTS(WRAP_MIXED, )

long
wrap_copied(int count, ...)
{
        va_list ap;
        long r;

        va_start(ap, count);
        r = copied(count, ap);
        va_end(ap);

        return r;
}

long
wrap_stepped(int count, ...)
{
        va_list ap;
        long r;

        va_start(ap, count);
        r = stepped(count, ap);
        va_end(ap);

        return r;
}

long
wrap_fetched(int count, ...)
{
        va_list ap;
        long r;

        va_start(ap, count);
        r = fetched(count, ap);
        va_end(ap);

        return r;
}
