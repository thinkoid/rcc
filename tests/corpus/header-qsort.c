/* A program with stdlib.h, the system's: qsort sorts an array with the
   program's own comparison function, whose address the program passes
   as the header's __compar_fn_t, a pointer to a function.  qsort is the
   C library's and calls the comparison through that pointer.  The
   program then checks that each element is no greater than the next,
   so its exit status says that the array is sorted, which it is only
   if the address went in as the function's and the library's calls
   through it reached the comparison.

   Under rcc and under clang -O2 it exits 0: nothing here is undefined,
   and the row is the same on both sides.

   On x86-64 the listing must show the address of compare, a function
   the file defines, formed PC-relative, leaq compare(%rip), %rax,
   stored to its argument slot as any pointer, loaded into rcx, and
   call qsort@PLT.

   On aarch64 the listing must show the address formed PC-relative,
   adrp x0, compare and add x0, x0, :lo12:compare, stored to its
   argument slot, loaded into x3, and bl qsort. */
#include <stdlib.h>

int
compare(const void *a, const void *b)
{
        const int *x = a;
        const int *y = b;

        return (*x > *y) - (*x < *y);
}

int
main(void)
{
        int a[6];
        int i;

        a[0] = 42;
        a[1] = -7;
        a[2] = 1000;
        a[3] = 0;
        a[4] = -7;
        a[5] = 3;

        qsort(a, 6, sizeof a[0], compare);

        for (i = 1; i < 6; i++)
                if (a[i - 1] > a[i])
                        return 1;

        return 0;
}
