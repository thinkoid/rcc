/* backend.md 7.2 and aarch64.md 7.2, a loop whose body ends in a jump:
   no path leaves the body at its end, so no jump back to the condition
   follows it.  while_returns' body ends in if (n > 5) break; else
   return 3;, while_breaks' in a break, and while_continues' in
   if (n & 1) continue; else return 3;, whose continue jumps to the
   condition itself.  while_falls is the edge: its body falls through
   past its if and keeps the jump back.  The for_ and do_ functions are
   the same bodies in a for and a do, and for_ever a for (;;) left only
   by its return: the for places its step's label after the body and
   the do its condition's, a continue's target, so each writes its step
   and jump back, or its condition, as after a body that falls through.

   On x86-64 the listing must show while_returns' jmp .Lwhile_returns.1
   followed at once by .Lwhile_returns.3, the label past the loop;
   while_breaks' jmp .Lwhile_breaks.3 followed at once by
   .Lwhile_breaks.3; while_continues' jne .Lwhile_continues.2 and its
   jmp .Lwhile_continues.1 followed at once by .Lwhile_continues.3; and
   while_falls' jmp .Lwhile_falls.2 after .Lwhile_falls.4.  In
   for_returns the step's label .Lfor_returns.3, the step and
   jmp .Lfor_returns.2; in do_returns .Ldo_returns.3 and the
   condition's jne .Ldo_returns.2.

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
