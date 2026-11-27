/* A C label is one more label of the function, .Lf.n as any other,
   numbered where the generation first meets it, docs/backend.md 7.2:
   at its definition, at a goto to it, or at an if whose then part is
   that goto, which asks the label before anything of the if.  The name
   is in no listing.

   Each function is one shape, its name the shape. */
int
forward(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto out;
        a = 1;
out:
        return a;
}

int
used_first(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (b)
                goto M;
L:
        a = a + 1;
        if (a < 3)
                goto L;
        c = 1;
M:
        return a;
}

int
defined_first(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
L:
        a = a + 1;
        if (a < 3)
                goto L;
        if (b)
                goto M;
        c = 1;
M:
        return a;
}

int
into_loop(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto in;
        while (a) {
        in:
                a = a - 1;
        }
        return a;
}

int
out_of_loop(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (1) {
                if (a)
                        goto done;
                a = a + 1;
        }
done:
        return a;
}

int
out_of_nest(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                while (b) {
                        if (c)
                                goto done;
                        b = b - 1;
                }
                a = a - 1;
        }
done:
        return a;
}

int
then_part_of_two_items(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a) {
                b = 1;
                goto L;
        }
        c = 2;
L:
        return b;
}

int
labeled_then_part(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
        L:
                b = 1;
        return b;
}

int
labeled_body(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a)
        L:
                a = a - 1;
        return a;
}

int
two_labels(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        goto M;
L:
        a = 1;
M:
        b = 2;
        goto L;
}

int
two_gotos(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        if (a)
                goto L;
        if (b)
                goto L;
        c = 1;
L:
        return c;
}

int
in_for(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 3; a = a + 1) {
                if (b)
                        goto next;
                c = c + 1;
        next:;
        }
        return c;
}

int
out_of_for(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 3; a = a + 1) {
                if (b)
                        continue;
                goto out;
        }
out:
        return a;
}

int
loop_of_goto(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        a = 0;
L:
        if (a < 10) {
                a = a + 1;
                goto L;
        }
        return a;
}

int
out_of_declared_for(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (int i = 0; i < 3; i = i + 1) {
                if (i == 1)
                        goto out;
        }
out:
        return 0;
}
