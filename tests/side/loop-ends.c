/* A break or a continue as the last statement of a loop's body: the
   jump is written, to the next line where the label it names follows,
   and the body ends in a jump, so the loop writes no jump back, a do no
   test and a for no step, but where a continue placed the label of
   the test or of the step, which a path reaches.  A while's continue
   at the end of its body is its jump back, and the while writes no
   second one.

   Each function is one loop whose body ends in a break or a continue,
   its name the loop and the jump. */
int
while_break(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                a = 1;
                break;
        }
        return a;
}

int
while_continue(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a) {
                a = a - 1;
                continue;
        }
        return 0;
}

int
do_continue(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do {
                a = a - 1;
                continue;
        } while (a);
        return a;
}

int
do_break(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do {
                a = 1;
                break;
        } while (a);
        return a;
}

int
for_continue(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a = a + 1) {
                c = 1;
                continue;
        }
        return c;
}

int
for_break(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (a = 0; a < 10; a = a + 1) {
                c = 1;
                break;
        }
        return c;
}

int
while_break_alone(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a)
                break;
        return 0;
}

int
while_continue_alone(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (a)
                continue;
        return 0;
}

int
do_break_alone(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                break;
        while (a);
        return 0;
}

int
do_continue_alone(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                continue;
        while (a);
        return 0;
}

int
do_continue_then_break(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do {
                if (b)
                        continue;
                break;
        } while (a);
        return 0;
}
