/* E1 op= E2 on objects of every integer type, 6.5.17.3: the operation
   in the common type of the two, or for <<= and >>= in E1's promoted
   type, E1's read extended to it by its own sign and the result stored
   at its width.  For its effect x86-64 writes + - & | ^ << >> on the
   object in memory at the object's width whatever the value's type,
   addb %al, -1(%rbp) for c += l, its immediate the constant at that
   width, addb $44 for c += 300, and sar or shr by the object's sign;
   * / % and every value go through a register, the object read at the
   operation's width, movsbq -1(%rbp), %rax, the operation, and the
   narrow store, and imul takes the object as its source where the
   widths agree.  The value is cut to the object's width where the
   operation is wider, so a widening consumer writes the extension of
   the stored bits, movb %al, -1(%rbp) and movsbq %al, %rax.  aarch64
   loads at the operation's width, operates, and stores at the object's.

   Each function is one shape, its name the shape: the consumer, e_ the
   effect, v_ the value returned at the object's type, vi_ as an int, vl_
   as a long, st_ stored into a long, cd_ a condition, ek_ the effect by a
   constant, vk_ its value, h_ another; the operator, add sub mul div rem
   shl shr and or xor; the object's type, then the value's or the
   constant's, c sc uc s us i u l ul for char to unsigned long, and m1
   for -1, imax 0x7fffffff, h80 0x80000000, big 0x123456789, hi
   0xffffffff00000000, the others as written. */

int g(int x);

int
h_ovf_rhs(void)
{
        int x;
        x += 2147483647 + 1;
        return 0;
}

int
e_add_c_l(void)
{
        char x;
        long y;
        x += y;
        return 0;
}

int
e_add_s_uc(void)
{
        short x;
        unsigned char y;
        x += y;
        return 0;
}

int
e_add_i_ul(void)
{
        int x;
        unsigned long y;
        x += y;
        return 0;
}

int
e_add_l_c(void)
{
        long x;
        char y;
        x += y;
        return 0;
}

int
e_add_l_u(void)
{
        long x;
        unsigned y;
        x += y;
        return 0;
}

int
e_add_ul_i(void)
{
        unsigned long x;
        int y;
        x += y;
        return 0;
}

int
e_shl_c_l(void)
{
        char x;
        long y;
        x <<= y;
        return 0;
}

int
e_shl_l_c(void)
{
        long x;
        char y;
        x <<= y;
        return 0;
}

int
e_shr_uc_c(void)
{
        unsigned char x;
        char y;
        x >>= y;
        return 0;
}

int
e_shr_c_uc(void)
{
        char x;
        unsigned char y;
        x >>= y;
        return 0;
}

int
e_shr_us_i(void)
{
        unsigned short x;
        int y;
        x >>= y;
        return 0;
}

int
e_shr_ul_l(void)
{
        unsigned long x;
        long y;
        x >>= y;
        return 0;
}

unsigned char
v_shr_uc_c(void)
{
        unsigned char x;
        char y;
        return x >>= y;
}

int
e_mul_c_c(void)
{
        char x;
        char y;
        x *= y;
        return 0;
}

int
e_mul_i_c(void)
{
        int x;
        char y;
        x *= y;
        return 0;
}

int
e_mul_i_l(void)
{
        int x;
        long y;
        x *= y;
        return 0;
}

int
e_mul_u_l(void)
{
        unsigned x;
        long y;
        x *= y;
        return 0;
}

int
e_mul_l_u(void)
{
        long x;
        unsigned y;
        x *= y;
        return 0;
}

int
ek_mul_c_5(void)
{
        char x;
        x *= 5;
        return 0;
}

int
ek_mul_i_5l(void)
{
        int x;
        x *= 5L;
        return 0;
}

int
ek_mul_l_5(void)
{
        long x;
        x *= 5;
        return 0;
}

int
e_div_c_c(void)
{
        char x;
        char y;
        x /= y;
        return 0;
}

int
e_div_c_u(void)
{
        char x;
        unsigned y;
        x /= y;
        return 0;
}

int
e_div_c_ul(void)
{
        char x;
        unsigned long y;
        x /= y;
        return 0;
}

int
e_div_uc_l(void)
{
        unsigned char x;
        long y;
        x /= y;
        return 0;
}

int
e_div_u_l(void)
{
        unsigned x;
        long y;
        x /= y;
        return 0;
}

int
e_div_i_l(void)
{
        int x;
        long y;
        x /= y;
        return 0;
}

int
e_div_ul_c(void)
{
        unsigned long x;
        char y;
        x /= y;
        return 0;
}

int
e_rem_s_ul(void)
{
        short x;
        unsigned long y;
        x %= y;
        return 0;
}

int
e_rem_l_u(void)
{
        long x;
        unsigned y;
        x %= y;
        return 0;
}

int
ek_div_c_5l(void)
{
        char x;
        x /= 5L;
        return 0;
}

int
ek_add_c_300(void)
{
        char x;
        x += 300;
        return 0;
}

int
ek_add_c_big(void)
{
        char x;
        x += 0x123456789;
        return 0;
}

int
ek_add_c_255(void)
{
        char x;
        x += 255;
        return 0;
}

int
ek_add_s_big(void)
{
        short x;
        x += 0x123456789;
        return 0;
}

int
ek_add_i_big(void)
{
        int x;
        x += 0x123456789;
        return 0;
}

int
ek_add_i_hi(void)
{
        int x;
        x += 0xffffffff00000000;
        return 0;
}

int
ek_add_i_h80(void)
{
        int x;
        x += 0x80000000;
        return 0;
}

int
ek_add_l_imax(void)
{
        long x;
        x += 0x7fffffff;
        return 0;
}

int
ek_add_l_h80(void)
{
        long x;
        x += 0x80000000;
        return 0;
}

int
ek_add_l_hi(void)
{
        long x;
        x += 0xffffffff00000000;
        return 0;
}

int
ek_and_l_hi(void)
{
        long x;
        x &= 0xffffffff00000000;
        return 0;
}

int
ek_or_i_big(void)
{
        int x;
        x |= 0x123456789;
        return 0;
}

int
ek_xor_s_65537(void)
{
        short x;
        x ^= 65537;
        return 0;
}

int
ek_add_c_5l(void)
{
        char x;
        x += 5L;
        return 0;
}

int
ek_add_c_65537(void)
{
        char x;
        x += 65537;
        return 0;
}

int
ek_add_l_65537(void)
{
        long x;
        x += 65537;
        return 0;
}

char
vk_add_c_300(void)
{
        char x;
        return x += 300;
}

int
vk_add_i_big(void)
{
        int x;
        return x += 0x123456789;
}

char
vk_add_c_big(void)
{
        char x;
        return x += 0x123456789;
}

int
h_shl_i_k32(void)
{
        int x;
        x <<= 32;
        return 0;
}

int
h_shl_ul_k64(void)
{
        unsigned long x;
        x <<= 64;
        return 0;
}

int
ek_shl_c_m1(void)
{
        char x;
        x <<= -1;
        return 0;
}

int
ek_shl_l_m1(void)
{
        long x;
        x <<= -1;
        return 0;
}

int
ek_shr_uc_300(void)
{
        unsigned char x;
        x >>= 300;
        return 0;
}

int
h_shr_us_k257(void)
{
        unsigned short x;
        x >>= 257;
        return 0;
}

int
h_shl_c_k8(void)
{
        char x;
        x <<= 8;
        return 0;
}

int
ek_add_c_0(void)
{
        char x;
        x += 0;
        return 0;
}

int
ek_mul_l_1(void)
{
        long x;
        x *= 1;
        return 0;
}

int
ek_and_c_255(void)
{
        char x;
        x &= 255;
        return 0;
}

long
h_and_l_km1(void)
{
        long x;
        return x &= -1;
}

int
h_and_ul_kmask(void)
{
        unsigned long x;
        x &= 0xffffffff;
        return 0;
}

int
ek_div_c_1(void)
{
        char x;
        x /= 1;
        return 0;
}

char
v_add_c_c(void)
{
        char x;
        char y;
        return x += y;
}

char
v_add_c_l(void)
{
        char x;
        long y;
        return x += y;
}

unsigned char
v_add_uc_l(void)
{
        unsigned char x;
        long y;
        return x += y;
}

unsigned short
v_add_us_l(void)
{
        unsigned short x;
        long y;
        return x += y;
}

int
v_add_i_l(void)
{
        int x;
        long y;
        return x += y;
}

unsigned
v_add_u_l(void)
{
        unsigned x;
        long y;
        return x += y;
}

long
v_add_l_c(void)
{
        long x;
        char y;
        return x += y;
}

short
v_sub_s_ul(void)
{
        short x;
        unsigned long y;
        return x -= y;
}

char
v_mul_c_u(void)
{
        char x;
        unsigned y;
        return x *= y;
}

char
v_rem_c_c(void)
{
        char x;
        char y;
        return x %= y;
}

int
v_rem_i_l(void)
{
        int x;
        long y;
        return x %= y;
}

unsigned long
v_rem_ul_u(void)
{
        unsigned long x;
        unsigned y;
        return x %= y;
}

char
v_shl_c_l(void)
{
        char x;
        long y;
        return x <<= y;
}

unsigned
v_shr_u_c(void)
{
        unsigned x;
        char y;
        return x >>= y;
}

int
vi_add_c_c(void)
{
        char x;
        char y;
        return x += y;
}

int
vi_add_uc_l(void)
{
        unsigned char x;
        long y;
        return x += y;
}

long
vl_add_c_c(void)
{
        char x;
        char y;
        return x += y;
}

long
vl_add_sc_sc(void)
{
        signed char x;
        signed char y;
        return x += y;
}

long
h_ret_us_us_l(void)
{
        unsigned short x;
        unsigned short y;
        return x += y;
}

long
vl_add_i_i(void)
{
        int x;
        int y;
        return x += y;
}

long
h_ret_i_u_l(void)
{
        int x;
        unsigned y;
        return x += y;
}

long
vl_add_i_l(void)
{
        int x;
        long y;
        return x += y;
}

long
vl_add_u_u(void)
{
        unsigned x;
        unsigned y;
        return x += y;
}

long
vl_add_u_i(void)
{
        unsigned x;
        int y;
        return x += y;
}

long
vl_add_u_l(void)
{
        unsigned x;
        long y;
        return x += y;
}

long
vl_div_c_c(void)
{
        char x;
        char y;
        return x /= y;
}

long
vl_rem_u_l(void)
{
        unsigned x;
        long y;
        return x %= y;
}

int
st_add_c_c(void)
{
        char x;
        char y;
        long z;
        z = x += y;
        return 0;
}

int
st_add_i_c(void)
{
        int x;
        char y;
        long z;
        z = x += y;
        return 0;
}

int
st_add_i_l(void)
{
        int x;
        long y;
        long z;
        z = x += y;
        return 0;
}

int
st_add_u_i(void)
{
        unsigned x;
        int y;
        long z;
        z = x += y;
        return 0;
}

int
st_add_u_l(void)
{
        unsigned x;
        long y;
        long z;
        z = x += y;
        return 0;
}

int
st_add_l_us(void)
{
        long x;
        unsigned short y;
        long z;
        z = x += y;
        return 0;
}

int
st_mul_i_u(void)
{
        int x;
        unsigned y;
        long z;
        z = x *= y;
        return 0;
}

int
st_mul_s_s(void)
{
        short x;
        short y;
        long z;
        z = x *= y;
        return 0;
}

int
h_sts_c_c(void)
{
        char x;
        char y;
        char z;
        z = x += y;
        return 0;
}

int
h_sts_us_l(void)
{
        unsigned short x;
        long y;
        unsigned short z;
        z = x += y;
        return 0;
}

int
h_arg_c(void)
{
        char x;
        char y;
        return g(x += y);
}

int
h_arg_l(void)
{
        long x;
        long y;
        return g(x += y);
}

int
cd_add_c_c(void)
{
        char x;
        char y;
        if (x += y)
                return 1;
        return 0;
}

int
cd_add_c_l(void)
{
        char x;
        long y;
        if (x += y)
                return 1;
        return 0;
}

int
cd_add_i_l(void)
{
        int x;
        long y;
        if (x += y)
                return 1;
        return 0;
}

int
cd_add_l_c(void)
{
        long x;
        char y;
        if (x += y)
                return 1;
        return 0;
}

int
cd_mul_c_c(void)
{
        char x;
        char y;
        if (x *= y)
                return 1;
        return 0;
}

int
cd_rem_c_l(void)
{
        char x;
        long y;
        if (x %= y)
                return 1;
        return 0;
}

int
cd_shl_c_l(void)
{
        char x;
        long y;
        if (x <<= y)
                return 1;
        return 0;
}

int
cd_shr_uc_c(void)
{
        unsigned char x;
        char y;
        if (x >>= y)
                return 1;
        return 0;
}

int
h_cmp_eq0_c(void)
{
        char x;
        char y;
        if ((x -= y) == 0)
                return 1;
        return 0;
}

int
h_cmp_eq0_s_l(void)
{
        short x;
        long y;
        if ((x -= y) == 0)
                return 1;
        return 0;
}

int
h_while_c(void)
{
        char x;
        while (x -= 1)
                ;
        return 0;
}

int
h_while_u_l(void)
{
        unsigned x;
        long y;
        while (x -= y)
                ;
        return 0;
}

int
h_cmp_c(void)
{
        char x;
        char y;
        return (x += y) < 5;
}

int
h_remcall_c(void)
{
        char x;
        int b;
        x %= g(b);
        return 0;
}

int
h_divcall_ul(void)
{
        unsigned long x;
        int b;
        x /= g(b);
        return 0;
}

int
h_divdiv_l(void)
{
        long x;
        long a;
        long c;
        x /= a / c;
        return 0;
}

int
h_divrem_l(void)
{
        long x;
        long a;
        long c;
        x /= a % c;
        return 0;
}

int
h_shrem_l(void)
{
        long x;
        long a;
        long c;
        x <<= a % c;
        return 0;
}

int
h_shcall_c(void)
{
        char x;
        int b;
        x <<= g(b);
        return 0;
}

int
h_held_div_c(void)
{
        char x;
        char y;
        int z;
        return z + (x /= y);
}

long
h_held_div_l(void)
{
        long x;
        long y;
        long z;
        return z + (x /= y);
}

unsigned long
h_held_rem_ul(void)
{
        unsigned long x;
        unsigned long y;
        unsigned long z;
        return z - (x %= y);
}

int
h_held_shl_c(void)
{
        char x;
        long y;
        int z;
        return z + (x <<= y);
}

long
h_held_shl_l(void)
{
        long x;
        int y;
        long z;
        return z + (x <<= y);
}

int
h_mulcall_l(void)
{
        long x;
        int b;
        x *= g(b);
        return 0;
}

int
h_addcall_c(void)
{
        char x;
        int b;
        x += g(b);
        return 0;
}

long
h_addcall_c_v(void)
{
        char x;
        int b;
        return x += g(b);
}

int
h_chain_c_l(void)
{
        char x;
        long y;
        int z;
        x += y += z;
        return 0;
}

int
h_chain_l_c(void)
{
        long x;
        char y;
        int z;
        x += y += z;
        return 0;
}

int
e_add_l_uc(void)
{
        long x;
        unsigned char y;
        x += y;
        return 0;
}
