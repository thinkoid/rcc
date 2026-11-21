/* The type of a conditional whose arms are a pointer to int and 0 cast
   to a pointer to void: 0 cast to void * is a null pointer constant,
   6.3.2.3p3, and the result has the type of the other operand, int *,
   6.5.16p7; 0 cast to a pointer to a qualified void is none, and the
   result is a pointer to void qualified with the qualifiers of both.
   is() selects by the type, 6.5.2.1p3.  It exits 42 when each holds,
   or 100 plus the number of the first check that failed.

   Under rcc and under clang -O2 it exits 42.

   On x86-64 the listing must show each test of a constant, the
   selection folded and negated, movl $0, %eax and cmpl $0, %eax, with
   no instruction of the conditional.

   On aarch64 the listing must show the same, mov w0, #0 and
   cbz w0. */
#define is(e, type) _Generic((e), type: 1, default: 0)

int c, *ip;

int
main(void)
{
        if (!is(c ? (void *) 0 : ip, int *))
                return 101;

        if (!is(c ? (const void *) 0 : ip, const void *))
                return 102;

        if (!is(c ? ip : (volatile void *) 0, volatile void *))
                return 103;

        return 42;
}
