/* The functions in the order of their first declarations, the
   block's statics in the order their bodies are parsed: the code f,
   g and the data g.s, f.s. */
int f(void);
int
g(void)
{
        static int s = 1;
        return s + f();
}
int
f(void)
{
        static int s = 2;
        return s;
}
