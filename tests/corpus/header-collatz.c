/* The Collatz walk from 27, each step chosen by a conditional whose
   operands are calls of void functions, which a conditional of two
   void operands makes possible, 6.5.16p3, p6.  halve() and triple()
   write the next value through a pointer and count the step; the step
   is the walk's for, n % 2 ? triple(&n) : halve(&n).  The body tallies
   the value as odd or even by a conditional of void calls under a cast
   to void, the left operand of a comma whose right one records the
   greatest value, and a nested conditional of void arms counts the
   values that are 1 modulo 4 among the odd.

   The walk reaches 1 in 111 steps, 41 of them from an odd value, 17 of
   those 1 modulo 4, its greatest value 9232.  The program writes the
   lines "27: 111 steps, 41 odd, 70 even, peak 9232" and "odd values 1
   modulo 4: 17", checks the counts with ==, and exits 111, or exits
   200 plus the number of the first check that failed.

   Under rcc and under clang -O2 it prints the lines and exits 111:
   nothing here is undefined.

   On x86-64 and on aarch64 the listing must show the for's step as the
   condition's jump to the second arm, each arm's call, and the jump of
   the first arm past the second, call triple@PLT; jmp and
   bl triple; b, with no join after the second arm's call, and the same
   in the body for odd() and even(). */
#include <stdio.h>

static long steps;
static long odd_values, even_values, quarter_values;
static long peak;

/* the step from an even value */
static void
halve(long *n)
{
        *n /= 2;
        steps++;
}

/* the step from an odd value */
static void
triple(long *n)
{
        *n = 3 * *n + 1;
        steps++;
}

static void
odd(void)
{
        odd_values++;
}

static void
even(void)
{
        even_values++;
}

static void
quarter(void)
{
        quarter_values++;
}

static void
nothing(void)
{
}

/* the greatest value so far */
static void
record(long n)
{
        if (n > peak)
                peak = n;
}

int
main(void)
{
        long n;

        for (n = 27; n != 1; n % 2 ? triple(&n) : halve(&n)) {
                (void) (n % 2 ? odd() : even()), record(n);
                n % 2 ? (n % 4 == 1 ? quarter() : nothing()) : nothing();
        }

        printf("27: %ld steps, %ld odd, %ld even, peak %ld\n", steps,
               odd_values, even_values, peak);
        printf("odd values 1 modulo 4: %ld\n", quarter_values);

        if (steps != 111 || odd_values != 41 || even_values != 70)
                return 201;
        if (peak != 9232)
                return 202;
        if (quarter_values != 17)
                return 203;

        return (int) steps;
}
