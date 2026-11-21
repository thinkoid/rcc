/* The barrier, __asm__ __volatile__ ("" : : : "memory"), is the null
   statement and writes nothing (shape.md chapter 5): c1 makes every
   load and store the source writes, in the written order, and holds no
   value across a statement.  Each function has a twin after it, the
   same body with ; in the barrier's place.  stored() stores to a
   global before the barrier; stored_cc() names the clobbers "memory"
   and "cc", and stored_bare() names none, its template alone;
   fenced() writes the barrier as a program's macro does, the one
   statement of a statement expression; held() writes that macro in a
   second statement expression, a statement whose value is dropped; and
   valued() stores the value of such a block.

   On x86-64 the listing of each function must be its twin's, byte for
   byte but for the function's name and its labels: stored() as
   movl $1, g(%rip) alone, held() as addl $1, pending(%rip) and the
   dropped value's movl $0, %eax.

   On aarch64 the same: each function's listing is its twin's, stored()
   as mov w0, #1, adrp x16, g and str w0, [x16, :lo12:g], held() as
   pending's load, add w0, w0, #1 and its store, then mov w0, #0. */
#define fence()      ({ __asm__ __volatile__("" : : : "memory"); })
#define fence_null() ({ ; })

#define hold               \
        ({                 \
                pending++; \
                fence();   \
                0;         \
        })
#define hold_null             \
        ({                    \
                pending++;    \
                fence_null(); \
                0;            \
        })

int g;
int pending;

void
stored(void)
{
        g = 1;
        __asm__ __volatile__("" : : : "memory");
}

void
stored_null(void)
{
        g = 1;
        ;
}

void
stored_cc(void)
{
        g = 1;
        __asm__ __volatile__("" : : : "memory", "cc");
}

void
stored_cc_null(void)
{
        g = 1;
        ;
}

void
stored_bare(void)
{
        g = 1;
        __asm__ __volatile__("");
}

void
stored_bare_null(void)
{
        g = 1;
        ;
}

void
fenced(void)
{
        g = 1;
        fence();
}

void
fenced_null(void)
{
        g = 1;
        fence_null();
}

void
held(void)
{
        hold;
}

void
held_null(void)
{
        hold_null;
}

int
valued(void)
{
        int x;

        x = ({
                g++;
                fence();
                0;
        });

        return x;
}

int
valued_null(void)
{
        int x;

        x = ({
                g++;
                fence_null();
                0;
        });

        return x;
}
