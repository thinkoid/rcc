/* One default label to a switch, 6.8.5.3p3, said at the second with
   the first's line. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        default:
        default: c = 1;
        }
        return c;
}
