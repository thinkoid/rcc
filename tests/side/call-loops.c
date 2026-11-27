/* Loops whose tests call.  The call is at the loop's head, after
   .p2align 5 on x86-64, and runs each time round, but its argument's
   slot is one slot, taken once, where the loop is generated.  A do
   tests its call at the end, jne back to the body; a for tests at its
   head.  An if of one jump inside a loop jumps out on the call's
   value, cmpl $0, %eax and jne, cbnz w0. */
int g(int a);

int
while_call(int a)
{
        while (g(a))
                a = a - 1;
        return a;
}

int
while_compared(int a, int b)
{
        while (g(a) < b)
                a = a + 1;
        return a;
}

int
do_call(int a)
{
        do
                a = a - 1;
        while (g(a));
        return a;
}

int
for_call(int a)
{
        int i;
        for (i = 0; g(i); i = i + 1)
                a = a + g(i);
        return a;
}

int
break_on_call(int a)
{
        while (a) {
                if (g(a))
                        break;
                a = a - 1;
        }
        return a;
}

int
continue_on_call(int a)
{
        while (a) {
                a = a - 1;
                if (g(a))
                        continue;
                a = a - 1;
        }
        return a;
}
