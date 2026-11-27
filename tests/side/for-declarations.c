/* A declaration as a for's init, 6.8.6.4p1: the for opens a scope of
   its own before it, so the name is in scope for the whole loop and
   gone after it, and a braced body's scope nests in the for's.  The
   declaration's slots are taken where the init is generated, before
   the loop's three labels, and never given back: after a, b, c the
   init's local is at -16 on x86-64 and #28 on aarch64, two declarators
   take -16 and -20, a second for the next slot, and so does a
   declaration after the loop.  A declaration without an initializer
   takes its slot and stores nothing.

   Each function is one shape, its name the shape. */
int
two_declarators(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0, j = 5; i < j; i = i + 1)
                a = a + j;
        return a;
}

int
no_initializer(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i; i < 3; i = i + 1)
                a = a + 1;
        return a;
}

int
two_loops(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0; i < 3; i = i + 1)
                a = a + i;
        for (int i = 0; i < 2; i = i + 1)
                b = b + i;
        return a + b;
}

int
nested(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0; i < 3; i = i + 1)
                for (int i = 0; i < 2; i = i + 1)
                        a = a + i;
        return a;
}

int
declaration_after(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0; i < 1; i = i + 1)
                ;
        int d = 3;
        return d;
}

int
shadows_local(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int a = 0; a < 3; a = a + 1)
                b = b + a;
        return a;
}

int
shadowed_in_body(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0; i < 3; i = i + 1) {
                int i = 5;
                a = a + i;
        }
        return a;
}

int
local_before(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        int i = 7;
        for (int i = 0; i < 3; i = i + 1)
                a = a + i;
        return i;
}

int
initializer_of_locals(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = a + b; i < c; i = i + 1)
                a = i;
        return a;
}

int
broken(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0;; i = i + 1)
                if (i > 3)
                        break;
        return a;
}

int
continued(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0; i < 3; i = i + 1) {
                if (i)
                        continue;
                a = a + 1;
        }
        return a;
}

long
of_long(void)
{
        int a = 1;
        long s = 0;
        for (long l = 4294967296; l > 0; l = l - 1000000007)
                s = s + l;
        return s;
}

int
of_char(void)
{
        int a = 1;
        for (char c = 'a', d; c < 'z'; c = c + 1) {
                d = c;
                a = a + d;
        }
        return a;
}

int
main(void)
{
        for (int i = 0; i < 3; i = i + 1)
                ;
}
