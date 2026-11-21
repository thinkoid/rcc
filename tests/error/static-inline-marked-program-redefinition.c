/* A static inline function a system header defines, its specifiers
   carrying a marking attribute, and the program defines again: the
   skipped body defines the function whatever words the attribute
   leaves on it, so the program's definition is a redefinition,
   6.9.1p3, not the attribute's not-yet.  The line markers say what
   clang -E says of an #include of a system header: the first
   definition is the text of one, header.h, by the flag 3, and the
   program's own text follows it. */
# 1 "header.h" 1 3
static inline __attribute__((__artificial__)) int
f(int x)
{
        return x;
}
# 15 __BASE_FILE__ 2

static int
f(int x)
{
        return x;
}
