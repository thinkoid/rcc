/* A constant condition is no special case: it is loaded and tested at
   the head of a while and a for and at the foot of a do, movl $1,
   %eax, cmpl $0, %eax and je on x86-64, mov w0, #1 and cbz w0 on
   aarch64, and the label past the loop is placed.  while (0) writes
   its body all the same.  Only an omitted condition writes no test.

   Each function is one loop on a constant, its name the loop and the
   constant. */
int
while_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (1) {
                if (a)
                        break;
                a = a + 1;
        }
        return a;
}

int
while_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (0)
                a = 1;
        return 0;
}

int
do_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                a = a + 1;
        while (1);
        return a;
}

int
do_zero(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        do
                a = a + 1;
        while (0);
        return a;
}

int
for_one(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (; 1;)
                a = 1;
        return a;
}

int
for_omitted(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        for (;;) {
                if (a)
                        break;
        }
        return 0;
}

int
while_seven(void)
{
        int a = 1;
        int b = 2;
        int c = 3;
        while (7)
                a = a + 1;
        return a;
}
