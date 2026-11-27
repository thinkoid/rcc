/* The data after the functions, one .data block of the objects with an
   initializer, then one .bss block of those without, each in the order
   of the objects' first declarations that define them: .data a, d, e,
   h and .bss b, c, g.  An object only declared writes nothing. */
int b;
int a = 1;
static int c;
int d = 2;
static int e = 3;
extern int x;

int
f(void)
{
        return 0;
}

int g;
int h = 4;
