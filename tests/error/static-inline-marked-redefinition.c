/* A static inline function defined twice in a system header, its
   specifiers carrying a marking attribute, as glibc writes
   __artificial__ on its fortified functions: the skipped body defines
   the function whatever words the attribute leaves on it, so the
   second definition is a redefinition, 6.9.1p3.  The line marker says
   what clang -E says of an #include of a system header: the
   definitions are the text of one, header.h, by the flag 3. */
# 1 "header.h" 1 3
static inline __attribute__((__artificial__)) int
f(int x)
{
        return x;
}

static inline __attribute__((__artificial__)) int
f(int x)
{
        return x;
}
