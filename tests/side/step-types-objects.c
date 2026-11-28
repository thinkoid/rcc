/* ++ and -- of an object of static storage of every integer type, the
   shapes of step-types.c with the stepped object at file scope:
   defined, its place a label, stepped in place for its effect, addb $1,
   d_e_pre_c(%rip); only declared, through the address the GOT holds,
   formed first and held across the load and the store.  Each function
   is its shape's in step-types.c, named defined_ or declared_ and the
   shape, and its object d_ or e_ and the shape. */

int g(int x);
char d_e_pre_c = 5;
extern char e_e_pre_c;
short d_e_postd_s = 5;
extern short e_e_postd_s;
long d_e_post_l = 5;
extern long e_e_post_l;
char d_v_pre_c = 5;
extern char e_v_pre_c;
char d_v_post_c = 5;
extern char e_v_post_c;
unsigned short d_v_post_us = 5;
extern unsigned short e_v_post_us;
long d_v_postd_l = 5;
extern long e_v_postd_l;
char d_st_post_c = 5;
extern char e_st_post_c;
short d_sts_post_s = 5;
extern short e_sts_post_s;
char d_vi_pre_c = 5;
extern char e_vi_pre_c;
unsigned char d_vi_pre_uc = 5;
extern unsigned char e_vi_pre_uc;
char d_vi_post_c = 5;
extern char e_vi_post_c;
char d_vl_pre_c = 5;
extern char e_vl_pre_c;
int d_vl_pre_i = 5;
extern int e_vl_pre_i;
unsigned d_vl_pre_u = 5;
extern unsigned e_vl_pre_u;
char d_vl_post_c = 5;
extern char e_vl_post_c;
signed char d_vl_post_sc = 5;
extern signed char e_vl_post_sc;
short d_vl_post_s = 5;
extern short e_vl_post_s;
int d_vl_post_i = 5;
extern int e_vl_post_i;
unsigned char d_vl_post_uc = 5;
extern unsigned char e_vl_post_uc;
unsigned char d_st_pre_uc = 5;
extern unsigned char e_st_pre_uc;
short d_st_post_s = 5;
extern short e_st_post_s;
char d_ad_pre_c = 5;
extern char e_ad_pre_c;
long d_ad_post_l = 5;
extern long e_ad_post_l;
unsigned char d_h_pre_eq_c_255 = 5;
extern unsigned char e_h_pre_eq_c_255;
char d_cd_pre_c = 5;
extern char e_cd_pre_c;
char d_cd_post_c = 5;
extern char e_cd_post_c;
short d_cd_post_s = 5;
extern short e_cd_post_s;
long d_cd_pre_l = 5;
extern long e_cd_pre_l;
short d_h_eq0_s = 5;
extern short e_h_eq0_s;
char d_h_while_c = 5;
extern char e_h_while_c;
unsigned long d_h_while_ul = 5;
extern unsigned long e_h_while_ul;
char d_h_post_held_c = 5;
extern char e_h_post_held_c;
char d_h_post_div_c = 5;
extern char e_h_post_div_c;
long d_h_post_div_l = 5;
extern long e_h_post_div_l;
char d_h_post_shl_c = 5;
extern char e_h_post_shl_c;
long d_h_post_shl_l = 5;
extern long e_h_post_shl_l;
char d_h_arg_post_c = 5;
extern char e_h_arg_post_c;

int
defined_e_pre_c(void)
{
        ++d_e_pre_c;
        return 0;
}

int
declared_e_pre_c(void)
{
        ++e_e_pre_c;
        return 0;
}

int
defined_e_postd_s(void)
{
        d_e_postd_s--;
        return 0;
}

int
declared_e_postd_s(void)
{
        e_e_postd_s--;
        return 0;
}

int
defined_e_post_l(void)
{
        d_e_post_l++;
        return 0;
}

int
declared_e_post_l(void)
{
        e_e_post_l++;
        return 0;
}

char
defined_v_pre_c(void)
{
        return ++d_v_pre_c;
}

char
declared_v_pre_c(void)
{
        return ++e_v_pre_c;
}

char
defined_v_post_c(void)
{
        return d_v_post_c++;
}

char
declared_v_post_c(void)
{
        return e_v_post_c++;
}

unsigned short
defined_v_post_us(void)
{
        return d_v_post_us++;
}

unsigned short
declared_v_post_us(void)
{
        return e_v_post_us++;
}

long
defined_v_postd_l(void)
{
        return d_v_postd_l--;
}

long
declared_v_postd_l(void)
{
        return e_v_postd_l--;
}

int
defined_st_post_c(void)
{
        long z;
        z = d_st_post_c++;
        return 0;
}

int
declared_st_post_c(void)
{
        long z;
        z = e_st_post_c++;
        return 0;
}

int
defined_sts_post_s(void)
{
        short z;
        z = d_sts_post_s++;
        return 0;
}

int
declared_sts_post_s(void)
{
        short z;
        z = e_sts_post_s++;
        return 0;
}

int
defined_vi_pre_c(void)
{
        return ++d_vi_pre_c;
}

int
declared_vi_pre_c(void)
{
        return ++e_vi_pre_c;
}

int
defined_vi_pre_uc(void)
{
        return ++d_vi_pre_uc;
}

int
declared_vi_pre_uc(void)
{
        return ++e_vi_pre_uc;
}

int
defined_vi_post_c(void)
{
        return d_vi_post_c++;
}

int
declared_vi_post_c(void)
{
        return e_vi_post_c++;
}

long
defined_vl_pre_c(void)
{
        return ++d_vl_pre_c;
}

long
declared_vl_pre_c(void)
{
        return ++e_vl_pre_c;
}

long
defined_vl_pre_i(void)
{
        return ++d_vl_pre_i;
}

long
declared_vl_pre_i(void)
{
        return ++e_vl_pre_i;
}

long
defined_vl_pre_u(void)
{
        return ++d_vl_pre_u;
}

long
declared_vl_pre_u(void)
{
        return ++e_vl_pre_u;
}

long
defined_vl_post_c(void)
{
        return d_vl_post_c++;
}

long
declared_vl_post_c(void)
{
        return e_vl_post_c++;
}

long
defined_vl_post_sc(void)
{
        return d_vl_post_sc++;
}

long
declared_vl_post_sc(void)
{
        return e_vl_post_sc++;
}

long
defined_vl_post_s(void)
{
        return d_vl_post_s++;
}

long
declared_vl_post_s(void)
{
        return e_vl_post_s++;
}

long
defined_vl_post_i(void)
{
        return d_vl_post_i++;
}

long
declared_vl_post_i(void)
{
        return e_vl_post_i++;
}

long
defined_vl_post_uc(void)
{
        return d_vl_post_uc++;
}

long
declared_vl_post_uc(void)
{
        return e_vl_post_uc++;
}

int
defined_st_pre_uc(void)
{
        long z;
        z = ++d_st_pre_uc;
        return 0;
}

int
declared_st_pre_uc(void)
{
        long z;
        z = ++e_st_pre_uc;
        return 0;
}

int
defined_st_post_s(void)
{
        long z;
        z = d_st_post_s++;
        return 0;
}

int
declared_st_post_s(void)
{
        long z;
        z = e_st_post_s++;
        return 0;
}

int
defined_ad_pre_c(void)
{
        int z;
        return ++d_ad_pre_c + z;
}

int
declared_ad_pre_c(void)
{
        int z;
        return ++e_ad_pre_c + z;
}

int
defined_ad_post_l(void)
{
        int z;
        return d_ad_post_l++ + z;
}

int
declared_ad_post_l(void)
{
        int z;
        return e_ad_post_l++ + z;
}

int
defined_h_pre_eq_c_255(void)
{
        return ++d_h_pre_eq_c_255 == 0;
}

int
declared_h_pre_eq_c_255(void)
{
        return ++e_h_pre_eq_c_255 == 0;
}

int
defined_cd_pre_c(void)
{
        if (++d_cd_pre_c)
                return 1;
        return 0;
}

int
declared_cd_pre_c(void)
{
        if (++e_cd_pre_c)
                return 1;
        return 0;
}

int
defined_cd_post_c(void)
{
        if (d_cd_post_c++)
                return 1;
        return 0;
}

int
declared_cd_post_c(void)
{
        if (e_cd_post_c++)
                return 1;
        return 0;
}

int
defined_cd_post_s(void)
{
        if (d_cd_post_s++)
                return 1;
        return 0;
}

int
declared_cd_post_s(void)
{
        if (e_cd_post_s++)
                return 1;
        return 0;
}

int
defined_cd_pre_l(void)
{
        if (++d_cd_pre_l)
                return 1;
        return 0;
}

int
declared_cd_pre_l(void)
{
        if (++e_cd_pre_l)
                return 1;
        return 0;
}

int
defined_h_eq0_s(void)
{
        if (++d_h_eq0_s == 0)
                return 1;
        return 0;
}

int
declared_h_eq0_s(void)
{
        if (++e_h_eq0_s == 0)
                return 1;
        return 0;
}

int
defined_h_while_c(void)
{
        while (--d_h_while_c)
                ;
        return 0;
}

int
declared_h_while_c(void)
{
        while (--e_h_while_c)
                ;
        return 0;
}

int
defined_h_while_ul(void)
{
        while (d_h_while_ul--)
                ;
        return 0;
}

int
declared_h_while_ul(void)
{
        while (e_h_while_ul--)
                ;
        return 0;
}

int
defined_h_post_held_c(void)
{
        int z;
        return z + d_h_post_held_c++;
}

int
declared_h_post_held_c(void)
{
        int z;
        return z + e_h_post_held_c++;
}

int
defined_h_post_div_c(void)
{
        int z;
        return z / d_h_post_div_c++;
}

int
declared_h_post_div_c(void)
{
        int z;
        return z / e_h_post_div_c++;
}

long
defined_h_post_div_l(void)
{
        long z;
        return z / d_h_post_div_l--;
}

long
declared_h_post_div_l(void)
{
        long z;
        return z / e_h_post_div_l--;
}

int
defined_h_post_shl_c(void)
{
        int z;
        return z << d_h_post_shl_c++;
}

int
declared_h_post_shl_c(void)
{
        int z;
        return z << e_h_post_shl_c++;
}

long
defined_h_post_shl_l(void)
{
        long z;
        return z << d_h_post_shl_l++;
}

long
declared_h_post_shl_l(void)
{
        long z;
        return z << e_h_post_shl_l++;
}

int
defined_h_arg_post_c(void)
{
        return g(d_h_arg_post_c++);
}

int
declared_h_arg_post_c(void)
{
        return g(e_h_arg_post_c++);
}
