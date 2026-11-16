/* backend.md 7.2 and aarch64.md 7.2, an if whose body is one jump: a
   break, a continue, a goto or a return with no value.  The if is the
   condition's jump to the body's target, taken when the condition
   holds, as the first pass of the Seventh Edition compiler writes it:
   no jump over the body and no label of the if's own.  With an else,
   the else part follows the jump.  low_else, even_else, clamp_else and
   early_else are low, even, clamp and early with an else; tally has a
   break and a continue in a switch in a for; low_braced is low with the
   break in braces, which write nothing.

   On x86-64 the listing must show the test of n & 1 in n's slot, testl
   $1, -4(%rbp) (backend.md 7.1), then jne .Llow.3 to the label past the
   while, and jne .Llow_else.3 followed at once by the else part's load
   of s; in even, jne .Leven.3 to the for's step; in clamp, jge
   .Lclamp.2 to done, numbered at the goto; in early, jne .Learly.1 to
   the epilogue; in tally, jg .Ltally.7 past the switch and jg .Ltally.3
   to the step; and low_braced as low.  No jmp follows any of these
   jumps, and no label.

   On aarch64 the listing must show the same jumps: and w0, w0, #0x1 and
   cbnz w0, .Llow.3; cbnz w0, .Leven.3; cmp w0, #0 and b.ge .Lclamp.2;
   cbnz w0, .Learly.1; b.gt .Ltally.7 and b.gt .Ltally.3; each followed
   by neither a b nor a label. */
int
low(int n)
{
        while (n) {
                if (n & 1)
                        break;
                n = n >> 1;
        }
        return n;
}

int
low_else(int n)
{
        int s = 0;
        while (n) {
                if (n & 1)
                        break;
                else
                        s = s + 1;
                n = n >> 1;
        }
        return s;
}

int
even(int n)
{
        int s = 0;
        for (int i = 0; i < n; i = i + 1) {
                if (i & 1)
                        continue;
                s = s + i;
        }
        return s;
}

int
even_else(int n)
{
        int s = 0;
        for (int i = 0; i < n; i = i + 1) {
                if (i & 1)
                        continue;
                else
                        s = s + i;
                s = s + 1;
        }
        return s;
}

int
clamp(int n)
{
        if (n >= 0)
                goto done;
        n = 0;
done:
        return n;
}

int
clamp_else(int n)
{
        if (n >= 0)
                goto done;
        else
                n = 0;
        n = n + 1;
done:
        return n;
}

void
early(int *p)
{
        if (*p)
                return;
        *p = 1;
}

void
early_else(int *p)
{
        if (*p)
                return;
        else
                *p = 1;
        *p = *p + 1;
}

int
tally(int n)
{
        int s = 0;
        for (int i = 0; i < n; i = i + 1) {
                switch (i & 3) {
                case 0:
                        if (s > 100)
                                break;
                        s = s + 1;
                        break;
                case 1:
                        if (i > 10)
                                continue;
                        s = s + 2;
                }
                s = s + 4;
        }
        return s;
}

int
low_braced(int n)
{
        while (n) {
                if (n & 1) {
                        break;
                }
                n = n >> 1;
        }
        return n;
}
