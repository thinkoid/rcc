/* A for with each of its three clauses present and omitted, 6.8.6.4:
   the init, then the head's label after the .p2align of x86-64, the
   test where the condition is written, the body, the step where it is
   written and the jump back, and the label past the loop where the
   test names it.  An omitted condition writes no test, and with no
   break nothing names the label past the loop, which is not placed:
   the code after the loop follows the jump back, and is written all
   the same.  An omitted init or step writes nothing.

   Each function is one for over c = c + 1;, its name the clauses it
   has. */
int
for_init_cond_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a = a + 1)
                c = c + 1;
        return c;
}

int
for_init_cond_no_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10;)
                c = c + 1;
        return c;
}

int
for_init_no_cond_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0;; a = a + 1)
                c = c + 1;
        return c;
}

int
for_init_no_cond_no_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0;;)
                c = c + 1;
        return c;
}

int
for_no_init_cond_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (; a < 10; a = a + 1)
                c = c + 1;
        return c;
}

int
for_no_init_cond_no_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (; a < 10;)
                c = c + 1;
        return c;
}

int
for_no_init_no_cond_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (;; a = a + 1)
                c = c + 1;
        return c;
}

int
for_no_init_no_cond_no_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (;;)
                c = c + 1;
        return c;
}
