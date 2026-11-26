/* Locals of type int and their initializers.  Each slot is taken where
   its declaration is met, 4 bytes above the last on aarch64 and below
   it on x86-64, and an initializer is stored into it as rcc's
   initialize() stores one: on x86-64 a constant is one movl into the
   slot, and anything else goes through a register; on aarch64 0 is the
   store of wzr, and anything else goes through a register.

   constant() initializes from a constant, zero() from 0, copy() from
   another local, and several() declares three in one declaration, the
   second without an initializer, the third from the first.  self()
   initializes x from x's own slot, which is in scope from its
   declarator on, 6.2.1p7; unused() declares a local it never reads,
   whose slot is still taken; uninitialized() reads a local nothing
   stored, G5's some value.  main() falls off its end after a
   declaration, so it returns 0. */
int
constant(void)
{
        int x = 7;
        return x;
}

int
zero(void)
{
        int x = 0;
        return x;
}

int
copy(void)
{
        int x = 100000;
        int y = x;
        return y;
}

int
several(void)
{
        int x = 1, y, z = x;
        return z;
}

int
self(void)
{
        int x = x;
        return x;
}

int
unused(void)
{
        int x = 5;
        return 6;
}

int
uninitialized(void)
{
        int x;
        return x;
}

int
main(void)
{
        int x = 0;
}
