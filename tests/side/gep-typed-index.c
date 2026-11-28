/* An index of the typed operators: a ?: of int arms, whose widening
   aarch64's form looks through, ldr w0, [x0, w1, sxtw #2] after the
   join, as it looks through a comma's ?:, and x86-64 hands to the arms,
   movslq on each path; a step and a compound assignment of a narrow
   object, promoted, then widened by the form; a ?: of long arms, 64
   bits as it is. */
int
arms(int *p, int c, int a, int b)
{
        return p[c ? a : b] + p[(a, c ? a : 3)] + p[c ? a < b : 2];
}

int
mixed(int *p, int c, unsigned u, int a)
{
        return p[c ? u : a] + p[c ? 1 : u];
}

int
steps(int *p, char c, short s, unsigned char u)
{
        return p[c++] + p[--s] + p[u += 2] + p[s -= 1];
}

int
wide(int *p, int c, long l, long m)
{
        return p[c ? l : m] + p[c ? l : 2];
}

void
stores(int *p, int c, int a, int b, char d)
{
        p[c ? a : b] = 1;
        p[d++] = c ? a : b;
}
