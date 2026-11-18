/* backend.md 7.2 and aarch64.md 7.2, a loop whose body ends in a jump:
   no path leaves the body at its end, so what follows the body in the
   loop is written only where a continue reaches it.  while_returns'
   body ends in if (n > 5) break; else return 3;, while_breaks' in a
   break, and while_continues' in if (n & 1) continue; else return 3;,
   whose continue jumps to the condition itself: none takes the jump
   back.  for_returns and do_returns are the first body in a for and a
   do: the for writes no step's label, step or jump back, the do no
   condition.  for_continues and do_continues are the third, whose
   continue reaches the step and the condition, so both are written.
   The _falls functions are the edge in each loop: the body falls
   through past its if and keeps what follows it, under no label, since
   no continue names one.  A label after a loop's head is placed only
   where a jump names it: for_ever, a for (;;) left only by its return,
   has neither the step's nor the one past the loop.  switch_returns
   is an if whose body is a switch with a default and no break, which
   places no label past it, so the if writes no jump over its else
   part; switch_breaks is the edge, whose break names the label and
   keeps the jump.

   On x86-64 the listing must show while_returns' jmp .Lwhile_returns.1
   followed at once by .Lwhile_returns.3, the label past the loop;
   while_breaks' jmp .Lwhile_breaks.3 followed at once by
   .Lwhile_breaks.3; while_continues' jne .Lwhile_continues.2 and its
   jmp .Lwhile_continues.1 followed at once by .Lwhile_continues.3; and
   while_falls' jmp .Lwhile_falls.2 after .Lwhile_falls.4.  In
   for_returns jmp .Lfor_returns.1 followed at once by .Lfor_returns.4,
   and no .Lfor_returns.3; in for_continues jne .Lfor_continues.3, then
   .Lfor_continues.3, the step and jmp .Lfor_continues.2.  In
   do_returns jmp .Ldo_returns.1 followed at once by .Ldo_returns.4, the
   break's label, and no .Ldo_returns.3; in do_continues
   .Ldo_continues.3 and the condition's jne .Ldo_continues.2.  for_ever's
   jmp .Lfor_ever.2 followed at once by .Lfor_ever.1.  switch_returns'
   jmp .Lswitch_returns.1, the default's return, followed at once by
   .Lswitch_returns.2, the else part's label; switch_breaks'
   .Lswitch_breaks.5 past the switch and jmp .Lswitch_breaks.6 over the
   else part.

   On aarch64 the listing must show the same, with b for jmp and the
   condition's branch for jne. */
int
while_returns(int n)
{
        while (n) {
                n = n - 1;
                if (n > 5)
                        break;
                else
                        return 3;
        }
        return n;
}

int
while_breaks(int n)
{
        while (n) {
                n = n - 1;
                break;
        }
        return n;
}

int
while_continues(int n)
{
        while (n) {
                n = n - 1;
                if (n & 1)
                        continue;
                else
                        return 3;
        }
        return n;
}

int
while_falls(int n)
{
        while (n) {
                n = n - 1;
                if (n > 5)
                        return 3;
        }
        return n;
}

int
for_returns(int n)
{
        for (int i = 0; i < n; i = i + 1) {
                if (i > 5)
                        break;
                else
                        return 3;
        }
        return n;
}

int
for_continues(int n)
{
        for (int i = 0; i < n; i = i + 1) {
                if (i & 1)
                        continue;
                else
                        return 3;
        }
        return n;
}

int
for_falls(int n)
{
        for (int i = 0; i < n; i = i + 1) {
                if (i > 5)
                        return 3;
        }
        return n;
}

int
for_ever(int n)
{
        for (;;) {
                n = n - 1;
                if (n == 0)
                        return 3;
        }
}

int
do_returns(int n)
{
        do {
                n = n - 1;
                if (n > 5)
                        break;
                else
                        return 3;
        } while (n);
        return n;
}

int
do_continues(int n)
{
        do {
                n = n - 1;
                if (n & 1)
                        continue;
                else
                        return 3;
        } while (n);
        return n;
}

int
do_falls(int n)
{
        do {
                n = n - 1;
                if (n > 5)
                        return 3;
        } while (n);
        return n;
}

int
switch_returns(int c, int n)
{
        if (c) {
                switch (n) {
                case 1: return 1;
                default: return 2;
                }
        }
        else
                n = 3;
        return n;
}

int
switch_breaks(int c, int n)
{
        if (c) {
                switch (n) {
                case 1: break;
                default: return 2;
                }
        }
        else
                n = 3;
        return n;
}
