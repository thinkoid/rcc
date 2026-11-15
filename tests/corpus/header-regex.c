/* A program with regex.h, the system's, written in GNU C, which rcc
   reads as it is: among its declarations regexec's parameter, which the
   preprocessor leaves as regmatch_t __pmatch[__restrict __nmatch], a
   qualifier inside a parameter's brackets and a size that names an
   earlier parameter.  sum is declared in the same shape, with const
   and static inside the brackets too, so its parameter a is a pointer
   to const int.  The program sums three elements through it and checks
   the sum, so its exit status says that the array reached sum as a
   pointer to its first element.

   Under rcc and under clang -O2 it exits 0: nothing here is undefined,
   and the row is the same on both sides.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with const int *a for sum's parameter
   and without the #include, and nothing for any declaration of the
   header.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
#include <regex.h>

int
sum(int n, const int a[const __restrict static n])
{
        int s = 0;
        int i;

        for (i = 0; i < n; i++)
                s += a[i];

        return s;
}

int
main(void)
{
        int a[3];

        a[0] = 1;
        a[1] = 2;
        a[2] = 3;

        if (sum(3, a) != 6)
                return 1;

        return 0;
}
