/* What a system header writes beside its declarations costs nothing,
   and an assembler name is the symbol a call names.  The attributes
   here, the qualifier __restrict, the function specifier __inline and
   __extension__ are dropped, so none of them writes anything (shape.md
   chapter 5).  twice is declared as glibc declares scanf, under
   another name in the assembly, and the call of twice calls
   other_name.  In local() an attribute begins a block's declarations
   and is read among their specifiers, dropped as one at file scope is.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program without the attributes, __restrict,
   __inline and __extension__, with twice declared and called as
   other_name.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
extern int count(const char *__restrict s) __attribute__ ((__nothrow__))
    __attribute__ ((__pure__, __nonnull__ (1)));
extern int twice(int x) __asm__ ("" "other_name") __attribute__ ((__const__));
__extension__ extern __inline int add(int *__restrict a, int *__restrict b)
    __attribute__ ((__nothrow__, __leaf__));

int use(const char *s, int *a, int *b)
{
    return count(s) + twice(*a) + add(a, b);
}

int
local(int n)
{
        __attribute__((__unused__)) int copy = n;
        __attribute__((__unused__)) static int calls;

        return copy + 1;
}
