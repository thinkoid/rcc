/* A case label in the scope of a variable length array its switch is
   outside of, 6.8.5.3p2: the array is declared in the switch's body
   before the label. */
int
f(int n, int c)
{
        switch (c) {
                char buf[n];
        case 1: buf[0] = 1; return buf[0];
        }
        return 0;
}

int
main(void)
{
        return f(1, 1);
}
