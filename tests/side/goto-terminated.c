/* A goto is the jump to its label, which sets what jmp and b set: no
   path reaches what is written next, so code after a goto is written
   all the same and a label after it starts a path again.  A then part
   that ends in a goto takes no jump over an else part; a dead if after
   a goto stays dead through its then part, so with no label there its
   then part falls into its else part with no jump, and with one the
   label opens a path and the jump over the else part is written.

   Each function is one shape, its name the shape. */
int
label_after_goto(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto L;
L:
        return 1;
}

int
dead_gotos(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto L;
        goto M;
L:
        return 1;
M:
        return 2;
}

int
dead_label(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        return 1;
L:
        a = 1;
        return a;
}

int
into_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto in;
        if (a)
                b = 1;
        else {
        in:
                b = 2;
        }
        return b;
}

int
into_then_with_else(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto in;
        if (a) {
        in:
                b = 1;
        }
        else
                b = 2;
        return b;
}

int
into_then(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto in;
        if (a) {
        in:
                b = 1;
        }
        return b;
}

int
then_part_label_else_goto(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
        L:
                b = 1;
        }
        else
                goto L;
        return b;
}

int
back_forever(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a = 0;
top:
        a = a + 1;
        goto top;
}
