/* The data of block's statics among objects at file scope, in the
   order their declarations are parsed: .data a, f.s, b and .bss f.t,
   g.s, c. */
int a = 1;
int
f(void)
{
        static int s = 2;
        static int t;
        return s + t;
}
int b = 3;
int
g(void)
{
        static int s;
        return s;
}
static int c;
