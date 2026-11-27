/* A case's constant reached through a signed overflow, 6.6p4: rcc warns
   at its first token, and the case is the value wrapped, -2147483648. */
int
f(int a)
{
        int b = 0;
        switch (a) {
        case 2147483647 + 1: b = 1; break;
        case 0: b = 2;
        }
        return b;
}
