/* In a void function, if (c) return h(); is not an if of one jump: its
   then part is the call for its effect and the return, two statements,
   so the if writes its own label, je .Lg.2, after the call and the jump
   to the epilogue.  rcc warns at the return, as for return h(); alone. */
void h(void);

void
g(int a)
{
        if (a)
                return h();
        a = 1;
}
