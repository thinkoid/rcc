/* The blocks probe: GNU C's statement expression in rcc's C, for
   docs/shape.md chapter 5 and docs/backend.md 6.7.  Each function
   takes two longs and returns a long; mark() records the calls, so the
   main compares the order of the statements' effects as well as the
   value.  See blocks.h. */

#include "blocks.h"

/* the value used: the block's own local in its slot */
long
used(long a, long b)
{
        long x = ({
                long t = a * 3;
                t + b;
        });

        return x;
}

/* a value held beside it while its block calls */
long
held(long a, long b)
{
        return a + ({
                       mark(1);
                       b;
               });
}

/* a statement expression inside one, each block calling with a value
   held beside it */
long
nested(long a, long b)
{
        return a * 2 + ({
                       long u = b + ({
                                        mark(2);
                                        a;
                                });
                       mark(3);
                       u;
               });
}

/* a break out of it, a value held beside it */
long
broken(long a, long b)
{
        long s = 0;
        long i;

        for (i = 0; i < 12; i++)
                s = s * 3 + a + ({
                            if (i == (b & 7))
                                    break;
                            mark(i);
                            i;
                    });

        return s * 100 + i;
}

/* a continue out of it, a value held beside it */
long
continued(long a, long b)
{
        long s = 0;
        long i;

        for (i = 0; i < 12; i++)
                s += (a & 0xff) * 5 + ({
                             if ((i ^ b) & 1)
                                     continue;
                             mark(i);
                             i;
                     });

        return s;
}

/* a return out of it, a value held beside it */
long
returned(long a, long b)
{
        return a * 7 + ({
                       if (b > a)
                               return mark(b - a);
                       a;
               });
}

/* in a for's condition and in its step */
long
looped(long a, long b)
{
        long n = 0;
        long i = 0;

        for (; ({
                     n += i;
                     i < (b & 15);
             });
             ({
                     i++;
                     n = n * 2 + mark(i);
             }))
                n ^= a;

        return n;
}

/* two in one expression, each declaring a register variable, which the
   second takes the register of the first's */
long
siblings(long a, long b)
{
        register long k = a ^ b;

        return ({
                       register long r = a + 1;
                       r;
               }) * 1000 +
               ({
                       register long q = b - 1;
                       q;
               }) +
               k;
}

/* arguments past the sixth and past the eighth */
long
arguments(long a, long b)
{
        return ten(1, 2, 3, 4, 5, 6, ({
                           mark(7);
                           a;
                   }),
                   8, ({
                           long t = b;
                           t * 2;
                   }),
                   10);
}

/* the read of a register variable held beside it, which its block
   writes on one path */
long
rewritten(long a, long b)
{
        register long x = a;

        return x + ({
                       if (b & 1)
                               x = b;
                       x * 2;
               });
}

/* a switch in its block, the cases inside it */
long
switched(long a, long b)
{
        return ({
                long r;
                switch (b & 3) {
                case 0: r = a; break;
                case 1: r = a + b; break;
                default: r = mark(b);
                }
                r;
        });
}

/* a static object of its block, which keeps its value from call to
   call */
long
counted(long a, long b)
{
        return ({
                       static long n;
                       n += a & 0xff;
                       n;
               }) +
               b;
}

/* a goto out of it, a value held beside it */
long
left(long a, long b)
{
        long s = a + ({
                         if (b < 0)
                                 goto out;
                         mark(4);
                         b;
                 });

        return s;

out:
        return -a;
}

/* as a condition */
long
tested(long a, long b)
{
        if (({
                    mark(5);
                    a > b;
            }))
                return 1;

        return 2;
}

/* narrow values, each the type of its last expression */
long
narrow(long a, long b)
{
        return ({
                       char c = a;
                       c;
               }) +
               ({
                       unsigned short u = b;
                       u;
               });
}

/* for its effect, its value dropped, then as an operand of a cast to
   void and of a comma */
long
effect(long a, long b)
{
        long n = a;

        ({
                n += b;
                mark(n);
        });
        (void) ({
                n *= 3;
                0;
        });

        return (({ n -= a; }), n);
}

/* into a register variable, by an assignment and by an initializer */
long
into_register(long a, long b)
{
        register long r = 0;

        r = ({
                long t = a;
                mark(1);
                t + b;
        });
        register long s = ({
                mark(2);
                a;
        });

        return r * 3 + s;
}

/* a compound assignment's right operand, of a register variable and of
   an object in memory */
long
compound(long a, long b)
{
        register long r = a;
        long m = a;

        r += ({
                mark(3);
                b;
        });
        m -= ({
                mark(4);
                b;
        });

        return r ^ m;
}

/* an element's place held across it */
long
placed(long a, long b)
{
        long v[4];

        v[a & 3] = ({
                mark(5);
                b;
        });

        return v[a & 3];
}

/* a shift's count */
long
shifted(long a, long b)
{
        return (a << ({
                        mark(6);
                        b & 31;
                })) ^
               (long) ((unsigned long) a >> ({ b & 63; }));
}

/* a division's divisor and a remainder's, kept from 0 and -1 */
long
divided(long a, long b)
{
        long d = (b & 0xff) | 1;

        return a / ({
                       mark(7);
                       d;
               }) +
               a % ({ d; });
}

/* the arms of ?:, the second declaring a register variable, and the
   arms into a register variable */
long
arms(long a, long b)
{
        register long r;

        r = a & 1 ? ({
                mark(8);
                a;
        }) :
                    ({ b; });

        return (a > b ? ({
                       mark(9);
                       a - b;
               }) :
                        ({
                                register long q = b;
                                q - a;
                        })) +
               r;
}

/* values held deep in an expression around it */
long
pressure(long a, long b)
{
        return a + (b + (a * 2 + (b * 3 + (a ^ ({
                                                   mark(10);
                                                   b;
                                           })))));
}

/* the operands of && and || */
long
logical(long a, long b)
{
        return (a && ({
                        mark(11);
                        b;
                })) ||
               ({
                       mark(12);
                       a > 0;
               });
}

/* a pointer as its value, dereferenced */
long
pointer(long a, long b)
{
        long v[2];

        v[0] = a;
        v[1] = b;

        return *({
                long *p = v;
                mark(13);
                p + (b & 1);
        });
}

/* a loop of a label and a goto in its block, a value held beside it */
long
labeled(long a, long b)
{
        return a * 3 + ({
                       long r = 0;
                       long n = b & 15;
               again:
                       if (n-- > 0) {
                               r += mark(n);
                               goto again;
                       }
                       r;
               });
}

/* a goto out of a statement expression to a label of the one around
   it, a value held beside each */
long
escaped(long a, long b)
{
        long n = 0;
        long t = 0;

        return a + ({
                       long s;
               next:
                       s = b * 2 + ({
                                   if (++n < (a & 7)) {
                                           t += mark(n);
                                           goto next;
                                   }
                                   n;
                           });
                       s + t;
               });
}
