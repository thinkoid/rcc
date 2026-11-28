/* The null pointer constants, 6.3.2.3p3: an integer constant
   expression of value 0 of any integer type, stored as the pointer's
   constant, movq $0, -8(%rbp) and str xzr, [x29, #16]; (int *) 0 is a
   null pointer, of the pointer's type already. */
int
f(void)
{
        int x = 1;
        int *p = &x;
        int *q = 0;
        int *r = (1 - 1);
        int *s = (int *) 0;
        int *t = 0L;
        int *u = '\0';
        q = 0;
        return p == q && q == r && r == s && s == t && t == u && p != 0L;
}
