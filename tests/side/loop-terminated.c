/* A loop's body that ends in a jump: no path leaves the body at its
   end, so a while writes no jump back and still places its label past
   the loop; a do writes no test, and nothing of the loop follows the
   body but the label past it where a break names it; a for writes no
   step and no jump back, but after the label a continue named.  The
   code after the loop is written all the same, and the loop's head is
   a label no jump may name.

   Each function is one loop whose body ends in a jump, its name the
   loop and the jump. */
int
while_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a)
                return 1;
        return 0;
}

int
while_both_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b)
                        return 1;
                else
                        return 2;
        }
        return 0;
}

int
do_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do {
                return 1;
        } while (a);
        return 0;
}

int
for_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a = a + 1)
                return 1;
        return 0;
}

int
for_omitted_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (;;)
                return 1;
        return 0;
}

int
for_continue_return(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (;; a = a + 1) {
                if (b)
                        continue;
                return 1;
        }
        return 0;
}

int
for_omitted_break_step(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (;; a = a + 1) {
                if (a)
                        break;
        }
        return 0;
}

int
while_break_continue(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                if (b)
                        break;
                if (c)
                        continue;
                a = 0;
        }
        return 0;
}
