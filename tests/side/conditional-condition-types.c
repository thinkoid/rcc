/* The condition of c ? y : n of any integer type, compared with 0 at
   its own width, 6.5.16p2: in memory on x86-64, cmpb $0, cmpw, cmpq; on
   aarch64 its load and cbz w0 or cbz x0.  A narrowing is the operand's
   load whole and the test of its low bits, cmpb $0, %al, tst w0, #0xff;
   a widening is tested at the wider width.  The same tests stand before
   a truth-valued arm's jump code. */
static short gs;
extern long gl;

int
char_condition(void)
{
        char c;
        int y;
        int n;
        return c ? y : n;
}

int
short_condition(void)
{
        short c;
        int y;
        int n;
        return c ? y : n;
}

int
long_condition(void)
{
        long c;
        int y;
        int n;
        return c ? y : n;
}

int
narrowed_to_char(void)
{
        int i;
        int y;
        int n;
        return (char) i ? y : n;
}

int
narrowed_to_unsigned_short(void)
{
        int i;
        int y;
        int n;
        return (unsigned short) i ? y : n;
}

int
widened(void)
{
        int i;
        int y;
        int n;
        return (long) i ? y : n;
}

int
long_narrowed_to_char(void)
{
        long l;
        int y;
        int n;
        return (char) l ? y : n;
}

int
long_narrowed_to_int(void)
{
        long l;
        int y;
        int n;
        return (int) l ? y : n;
}

int
jump_on_arms(void)
{
        char c;
        int y;
        int n;
        if (c ? y < n : n)
                return 1;
        return 0;
}

int
objects(void)
{
        int y;
        int n;
        return gs ? y : n;
}

int
object_declared(void)
{
        int y;
        return gl ? y : 2;
}

int
object_declared_jump(void)
{
        int y;
        int n;
        if (gl ? y < n : gs)
                return 1;
        return 0;
}
