/* The stack probe: variable length arrays in rcc's C, for
   docs/shape.md chapter 5 and G23.  Each function takes two longs and
   returns a long.  Each size is cut to a few hundred bytes at most,
   and an address is compared only with another of the same build, so
   both builds return the same.  See stack.h. */

#include "stack.h"

/* elements written here and read back, here and by clang's code */
long
element(long a, long b)
{
        long n = (a & 31) + 1;
        long v[n];

        for (long i = 0; i < n; i++)
                v[i] = b ^ i;

        return sum(v, n) + v[(b & 0xff) % n];
}

/* sizeof reads the size of the instance, whatever its operand holds
   later */
long
sized(long a, long b)
{
        long n = (a & 63) + 1;
        char c[n];
        int m[n];

        n = b;
        c[0] = 1;
        m[0] = 2;

        return (long) (sizeof c * 1000 + sizeof m) + c[0] + m[0] + (n & 1);
}

/* a call with arguments on the stack, below the storage, its elements
   the arguments */
long
stacked(long a, long b)
{
        long n = (a & 15) + 10;
        long v[n];

        for (long i = 0; i < n; i++)
                v[i] = b ^ (i * 3);

        return ten(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9]) +
               sum(v, n);
}

/* the stack pointer a multiple of 16 at a call, whatever the size, and
   the storage's address one too */
long
aligned(long a, long b)
{
        char c[(a & 63) + 1];

        c[0] = b & 0x7f;

        return frame_misalignment() * 100 +
               (long) ((unsigned long) c & 15) * 10 + c[0];
}

/* a loop of many turns, the storage given back at the end of each: the
   address does not move */
long
turns(long a, long b)
{
        long n = (a & 255) + 1;
        unsigned long first = 0;
        long moved = 0;

        for (long i = 0; i < 4096; i++) {
                char c[n];

                c[n - 1] = b & 0x7f;
                if (i == 0)
                        first = (unsigned long) c;
                moved += (unsigned long) c != first;
        }

        return moved * 1000 + (b & 7);
}

/* a break out of the scope: the next array takes the same address */
long
broken(long a, long b)
{
        long n = (a & 63) + 1;
        unsigned long first = 0;
        unsigned long later;

        for (long i = 0;; i++) {
                char c[n];

                c[0] = 1;
                first = (unsigned long) c;
                if (i == (b & 7))
                        break;
        }

        {
                char d[n];

                d[0] = 2;
                later = (unsigned long) d;
        }

        return later == first;
}

/* a continue out of the scope each other turn */
long
continued(long a, long b)
{
        long n = (b & 127) + 1;
        unsigned long first = 0;
        long moved = 0;
        long kept = 0;

        for (long i = 0; i < 64; i++) {
                char c[n];

                if (i == 0)
                        first = (unsigned long) c;
                moved += (unsigned long) c != first;
                if ((i ^ a) & 1)
                        continue;
                kept++;
        }

        return moved * 1000 + kept;
}

/* a goto out of the scope, on one path, and the block's end on the
   other */
long
left(long a, long b)
{
        long n = (a & 63) + 1;
        unsigned long first;
        unsigned long later;

        {
                char c[n];

                first = (unsigned long) c;
                if (b & 1)
                        goto out;
                c[0] = 3;
        }
out:
        {
                char d[n];

                later = (unsigned long) d;
        }

        return (later == first) * 10 + (b & 1);
}

/* a goto back before the declaration, in its block: the storage is
   given back before the declaration is reached again */
long
back(long a, long b)
{
        long n = (a & 63) + 1;
        unsigned long first = 0;
        long moved = 0;
        long k = 0;

        {
        again:
                k++;
                char c[n];

                c[0] = (char) k;
                if (k == 1)
                        first = (unsigned long) c;
                moved += (unsigned long) c != first;
                if (k < 64 + (b & 63))
                        goto again;
        }

        return moved * 1000 + k;
}

/* a statement expression's value, computed before its storage is given
   back */
long
valued(long a, long b)
{
        unsigned long first;
        unsigned long later;

        long v = ({
                long n = (a & 15) + 1;
                long w[n];

                for (long i = 0; i < n; i++)
                        w[i] = b ^ i;
                first = (unsigned long) w;
                sum(w, n);
        });

        {
                long z[(a & 15) + 1];

                later = (unsigned long) z;
        }

        return v * 2 + (later == first);
}

/* a statement expression as a condition, its value read before its
   storage is given back and tested after */
long
tested(long a, long b)
{
        unsigned long first;
        unsigned long later;
        long r;

        if (({
                    long n = (a & 15) + 1;
                    char c[n];

                    c[n - 1] = b & 1;
                    first = (unsigned long) c;
                    c[n - 1];
            }))
                r = 1;
        else
                r = 2;

        {
                char d[(a & 15) + 1];

                later = (unsigned long) d;
        }

        return r * 10 + (later == first);
}

/* a statement expression for its effect */
long
effect(long a, long b)
{
        long s = 0;
        unsigned long first;
        unsigned long later;

        ({
                long n = (b & 15) + 1;
                long w[n];

                for (long i = 0; i < n; i++)
                        w[i] = a ^ i;
                first = (unsigned long) w;
                s = sum(w, n);
        });

        {
                long z[(b & 15) + 1];

                later = (unsigned long) z;
        }

        return s + (later == first);
}

/* an array a for declares, in scope for the whole loop and given back
   past it */
long
declared(long a, long b)
{
        long n = (a & 63) + 8;
        unsigned long first = 0;
        unsigned long later;
        long k = 0;

        for (char c[n]; k < (b & 7); k++) {
                c[k] = (char) k;
                first = (unsigned long) c;
        }

        {
                char d[n];

                later = (unsigned long) d;
        }

        return (later == first) * 10 + k;
}

/* two arrays of one block: the second below the first by its size
   rounded up to 16, and the next array at the first's address once the
   block's end gives back both */
long
two(long a, long b)
{
        long n = (a & 31) + 1;
        long m = (b & 31) + 1;
        unsigned long first;
        unsigned long apart;
        unsigned long later;
        long s;

        {
                long x[n];
                long y[m];

                for (long i = 0; i < n; i++)
                        x[i] = a ^ i;
                for (long i = 0; i < m; i++)
                        y[i] = b ^ i;
                s = sum(x, n) * 3 + sum(y, m);
                first = (unsigned long) x;
                apart = (unsigned long) x - (unsigned long) y;
        }

        {
                char z[n * 8];

                later = (unsigned long) z;
        }

        return s + (long) apart * 7 + (later == first);
}

/* a goto back between two arrays of one block: the second's storage is
   given back each time around and the first's is kept, which the
   second's elements and a call writing below the stack pointer would
   overwrite */
long
between(long a, long b)
{
        long n = (a & 31) + 1;
        long m = (b & 31) + 1;
        long k = 0;
        long s = 0;
        long t;

        {
                long x[n];

                for (long i = 0; i < n; i++)
                        x[i] = a ^ i;
        again:
                k++;
                long y[m];

                for (long i = 0; i < m; i++)
                        y[i] = k + i;
                s += scribble(y[0]) + sum(y, m);
                if (k < 8)
                        goto again;

                t = s + sum(x, n);
        }

        return t;
}

/* an array in a nested block, given back at the block's end, where the
   next array takes its address */
long
nested(long a, long b)
{
        long n = (a & 31) + 1;
        long x[n];
        unsigned long inner;
        unsigned long later;
        long s;

        for (long i = 0; i < n; i++)
                x[i] = b ^ i;

        {
                long y[n];

                for (long i = 0; i < n; i++)
                        y[i] = x[i] + 1;
                s = sum(y, n);
                inner = (unsigned long) y;
        }

        {
                long z[n];

                z[0] = s;
                later = (unsigned long) z;
        }

        return s + sum(x, n) + (later == inner);
}
