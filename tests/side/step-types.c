/* ++ and -- of an object of every integer type, 6.5.4.1, 6.5.3.5: no
   promotion and no conversion in the tree.  For its effect x86-64 steps
   the object in memory at its width, addb $1, -1(%rbp), and aarch64
   loads, steps at 32 bits or 64 and stores at the object's width.  For
   its value the operation is at the promoted width, addl $1, %eax on a
   char, and a prefix value is cut to the object's width, so a consumer
   that widens it writes the extension of the stored bits, movb %al,
   -1(%rbp) and movsbl %al, %eax; a postfix value is the old one, as its
   load left it.  As a condition a prefix step on x86-64 jumps on the
   flags of the step on the object, and a postfix one compares its old
   value at the object's width, cmpb $0, %cl.

   Each function is one shape, its name the shape: the consumer, e_ the
   effect, v_ the value returned at the object's type, vi_ as an int,
   vl_ as a long, st_ stored into a long, sts_ into the object's type,
   cd_ a condition, ad_ added to an int, h_ another; the step, pre post
   pred postd for ++x x++ --x x--; the object's type, c sc uc s us i u l
   ul for char to unsigned long. */

int g(int x);

int
e_pre_c(void)
{
        char x;
        ++x;
        return 0;
}

int
e_postd_s(void)
{
        short x;
        x--;
        return 0;
}

int
e_post_l(void)
{
        long x;
        x++;
        return 0;
}

char
v_pre_c(void)
{
        char x;
        return ++x;
}

char
v_post_c(void)
{
        char x;
        return x++;
}

unsigned short
v_post_us(void)
{
        unsigned short x;
        return x++;
}

long
v_postd_l(void)
{
        long x;
        return x--;
}

int
st_post_c(void)
{
        char x;
        long z;
        z = x++;
        return 0;
}

int
sts_post_s(void)
{
        short x;
        short z;
        z = x++;
        return 0;
}

int
vi_pre_c(void)
{
        char x;
        return ++x;
}

int
vi_pre_uc(void)
{
        unsigned char x;
        return ++x;
}

int
vi_post_c(void)
{
        char x;
        return x++;
}

long
vl_pre_c(void)
{
        char x;
        return ++x;
}

long
vl_pre_i(void)
{
        int x;
        return ++x;
}

long
vl_pre_u(void)
{
        unsigned x;
        return ++x;
}

long
vl_post_c(void)
{
        char x;
        return x++;
}

long
vl_post_sc(void)
{
        signed char x;
        return x++;
}

long
vl_post_s(void)
{
        short x;
        return x++;
}

long
vl_post_i(void)
{
        int x;
        return x++;
}

long
vl_post_uc(void)
{
        unsigned char x;
        return x++;
}

int
st_pre_uc(void)
{
        unsigned char x;
        long z;
        z = ++x;
        return 0;
}

int
st_post_s(void)
{
        short x;
        long z;
        z = x++;
        return 0;
}

int
ad_pre_c(void)
{
        char x;
        int z;
        return ++x + z;
}

int
ad_post_l(void)
{
        long x;
        int z;
        return x++ + z;
}

int
h_pre_eq_c_255(void)
{
        unsigned char x;
        return ++x == 0;
}

int
cd_pre_c(void)
{
        char x;
        if (++x)
                return 1;
        return 0;
}

int
cd_post_c(void)
{
        char x;
        if (x++)
                return 1;
        return 0;
}

int
cd_post_s(void)
{
        short x;
        if (x++)
                return 1;
        return 0;
}

int
cd_pre_l(void)
{
        long x;
        if (++x)
                return 1;
        return 0;
}

int
h_eq0_s(void)
{
        short x;
        if (++x == 0)
                return 1;
        return 0;
}

int
h_while_c(void)
{
        char x;
        while (--x)
                ;
        return 0;
}

int
h_while_ul(void)
{
        unsigned long x;
        while (x--)
                ;
        return 0;
}

int
h_post_held_c(void)
{
        char x;
        int z;
        return z + x++;
}

int
h_post_div_c(void)
{
        char x;
        int z;
        return z / x++;
}

long
h_post_div_l(void)
{
        long x;
        long z;
        return z / x--;
}

int
h_post_shl_c(void)
{
        char x;
        int z;
        return z << x++;
}

long
h_post_shl_l(void)
{
        long x;
        long z;
        return z << x++;
}

int
h_arg_post_c(void)
{
        char x;
        return g(x++);
}
