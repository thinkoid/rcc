/* Functions of result type void, 6.9.1p3.  return; is the jump to the
   one epilogue, 6.8.7.5, in the middle of a body too, where the
   statements after it are written all the same; a body that no return
   ends falls off its closing brace with no diagnostic, an int
   function's too, whose result register holds what it holds, G5.  ()
   declares no parameter, as (void) does, 6.7.7.4p13. */
void
middle(void)
{
        int a = 1;
        return;
        a = 2;
}

void
last(void)
{
        int a = 1;
        a = a + 1;
        return;
}

void
falls(void)
{
        int a = 1;
}

void
empty()
{
}

int
falls_int(void)
{
        int a = 1;
}
