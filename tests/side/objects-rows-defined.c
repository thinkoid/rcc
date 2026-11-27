/* The rows of the survey's part B with the objects g and h defined
   by tentative definitions: each access names the object's label, a
   slot with another spelling, g(%rip) on x86-64 and adrp x16, g with
   [x16, :lo12:g] on aarch64, the adrp right before its access. */
int k(int x);
int k2(int x, int y);
int g;
int h;

int
load(void)
{
        int a;
        int b;
        int c;
        return g;
}

int
load_discard(void)
{
        int a;
        int b;
        int c;
        g;
        return 0;
}

int
store_local(void)
{
        int a;
        int b;
        int c;
        g = a;
        return 0;
}

int
store_const(void)
{
        int a;
        int b;
        int c;
        g = 7;
        return 0;
}

int
store_zero(void)
{
        int a;
        int b;
        int c;
        g = 0;
        return 0;
}

int
store_big(void)
{
        int a;
        int b;
        int c;
        g = 100000;
        return 0;
}

int
store_neg(void)
{
        int a;
        int b;
        int c;
        g = -1;
        return 0;
}

int
store_global(void)
{
        int a;
        int b;
        int c;
        g = h;
        return 0;
}

int
store_self(void)
{
        int a;
        int b;
        int c;
        g = g;
        return 0;
}

int
to_local(void)
{
        int a;
        int b;
        int c;
        a = g;
        return a;
}

int
init_local(void)
{
        int a;
        int b;
        int c;
        int x = g;
        return x;
}

int
init_local_expr(void)
{
        int a;
        int b;
        int c;
        int x = g + 1;
        return x;
}

int
ret_assign_const(void)
{
        int a;
        int b;
        int c;
        return g = 7;
}

int
ret_assign_zero(void)
{
        int a;
        int b;
        int c;
        return g = 0;
}

int
ret_assign_local(void)
{
        int a;
        int b;
        int c;
        return g = a;
}

int
ret_assign_global(void)
{
        int a;
        int b;
        int c;
        return g = h;
}

int
chain_local(void)
{
        int a;
        int b;
        int c;
        a = g = 7;
        return a;
}

int
chain_global(void)
{
        int a;
        int b;
        int c;
        g = h = 7;
        return 0;
}

int
chain_into(void)
{
        int a;
        int b;
        int c;
        g = a = 7;
        return 0;
}

int
chain_zero(void)
{
        int a;
        int b;
        int c;
        g = h = 0;
        return 0;
}

int
store_expr(void)
{
        int a;
        int b;
        int c;
        g = a + b;
        return 0;
}

int
add_ag(void)
{
        int a;
        int b;
        int c;
        return a + g;
}

int
add_ga(void)
{
        int a;
        int b;
        int c;
        return g + a;
}

int
add_gh(void)
{
        int a;
        int b;
        int c;
        return g + h;
}

int
add_gc(void)
{
        int a;
        int b;
        int c;
        return g + 5;
}

int
add_cg(void)
{
        int a;
        int b;
        int c;
        return 5 + g;
}

int
add_gg(void)
{
        int a;
        int b;
        int c;
        return g + g;
}

int
sub_ag(void)
{
        int a;
        int b;
        int c;
        return a - g;
}

int
sub_ga(void)
{
        int a;
        int b;
        int c;
        return g - a;
}

int
sub_gh(void)
{
        int a;
        int b;
        int c;
        return g - h;
}

int
sub_gc(void)
{
        int a;
        int b;
        int c;
        return g - 5;
}

int
sub_cg(void)
{
        int a;
        int b;
        int c;
        return 5 - g;
}

int
and_ag(void)
{
        int a;
        int b;
        int c;
        return a & g;
}

int
and_ga(void)
{
        int a;
        int b;
        int c;
        return g & a;
}

int
and_gc(void)
{
        int a;
        int b;
        int c;
        return g & 255;
}

int
or_ag(void)
{
        int a;
        int b;
        int c;
        return a | g;
}

int
or_gc(void)
{
        int a;
        int b;
        int c;
        return g | 1;
}

int
xor_ag(void)
{
        int a;
        int b;
        int c;
        return a ^ g;
}

int
xor_gc(void)
{
        int a;
        int b;
        int c;
        return g ^ 5;
}

int
mul_ag(void)
{
        int a;
        int b;
        int c;
        return a * g;
}

int
mul_ga(void)
{
        int a;
        int b;
        int c;
        return g * a;
}

int
mul_gh(void)
{
        int a;
        int b;
        int c;
        return g * h;
}

int
mul_gc(void)
{
        int a;
        int b;
        int c;
        return g * 5;
}

int
mul_cg(void)
{
        int a;
        int b;
        int c;
        return 5 * g;
}

int
mul_gc_store(void)
{
        int a;
        int b;
        int c;
        a = g * 5;
        return a;
}

int
mul_gc_into(void)
{
        int a;
        int b;
        int c;
        g = g * 5;
        return 0;
}

int
neg_g(void)
{
        int a;
        int b;
        int c;
        return -g;
}

int
not_g(void)
{
        int a;
        int b;
        int c;
        return ~g;
}

int
plus_g(void)
{
        int a;
        int b;
        int c;
        return +g;
}

int
lognot_g(void)
{
        int a;
        int b;
        int c;
        return !g;
}

int
div_ag(void)
{
        int a;
        int b;
        int c;
        return a / g;
}

int
div_ga(void)
{
        int a;
        int b;
        int c;
        return g / a;
}

int
div_gh(void)
{
        int a;
        int b;
        int c;
        return g / h;
}

int
div_gc(void)
{
        int a;
        int b;
        int c;
        return g / 5;
}

int
div_cg(void)
{
        int a;
        int b;
        int c;
        return 5 / g;
}

int
rem_ag(void)
{
        int a;
        int b;
        int c;
        return a % g;
}

int
rem_ga(void)
{
        int a;
        int b;
        int c;
        return g % a;
}

int
rem_gh(void)
{
        int a;
        int b;
        int c;
        return g % h;
}

int
rem_store(void)
{
        int a;
        int b;
        int c;
        g = a % b;
        return 0;
}

int
div_store(void)
{
        int a;
        int b;
        int c;
        g = a / b;
        return 0;
}

int
shl_ag(void)
{
        int a;
        int b;
        int c;
        return a << g;
}

int
shl_ga(void)
{
        int a;
        int b;
        int c;
        return g << a;
}

int
shl_gh(void)
{
        int a;
        int b;
        int c;
        return g << h;
}

int
shl_gc(void)
{
        int a;
        int b;
        int c;
        return g << 3;
}

int
shr_gh(void)
{
        int a;
        int b;
        int c;
        return g >> h;
}

int
shl_store(void)
{
        int a;
        int b;
        int c;
        g = a << b;
        return 0;
}

int
inc(void)
{
        int a;
        int b;
        int c;
        g = g + 1;
        return 0;
}

int
inc_ret(void)
{
        int a;
        int b;
        int c;
        return g = g + 1;
}

int
inc_h(void)
{
        int a;
        int b;
        int c;
        g = h + g;
        return 0;
}

int
swap3(void)
{
        int a;
        int b;
        int c;
        a = g;
        g = h;
        h = a;
        return 0;
}

int
lt_ag(void)
{
        int a;
        int b;
        int c;
        return a < g;
}

int
lt_ga(void)
{
        int a;
        int b;
        int c;
        return g < a;
}

int
lt_gh(void)
{
        int a;
        int b;
        int c;
        return g < h;
}

int
lt_gc(void)
{
        int a;
        int b;
        int c;
        return g < 5;
}

int
lt_cg(void)
{
        int a;
        int b;
        int c;
        return 5 < g;
}

int
eq_gc(void)
{
        int a;
        int b;
        int c;
        return g == 0;
}

int
ne_gh(void)
{
        int a;
        int b;
        int c;
        return g != h;
}

int
and_gh(void)
{
        int a;
        int b;
        int c;
        return g && h;
}

int
or_gh(void)
{
        int a;
        int b;
        int c;
        return g || h;
}

int
if_g(void)
{
        int a;
        int b;
        int c;
        if (g)
                return 1;
        return 0;
}

int
if_notg(void)
{
        int a;
        int b;
        int c;
        if (!g)
                return 1;
        return 0;
}

int
if_eq_gc(void)
{
        int a;
        int b;
        int c;
        if (g == 5)
                return 1;
        return 0;
}

int
if_eq_cg(void)
{
        int a;
        int b;
        int c;
        if (5 == g)
                return 1;
        return 0;
}

int
if_lt_gc(void)
{
        int a;
        int b;
        int c;
        if (g < 5)
                return 1;
        return 0;
}

int
if_lt_ga(void)
{
        int a;
        int b;
        int c;
        if (g < a)
                return 1;
        return 0;
}

int
if_lt_ag(void)
{
        int a;
        int b;
        int c;
        if (a < g)
                return 1;
        return 0;
}

int
if_lt_gh(void)
{
        int a;
        int b;
        int c;
        if (g < h)
                return 1;
        return 0;
}

int
if_test_gc(void)
{
        int a;
        int b;
        int c;
        if (g & 1)
                return 1;
        return 0;
}

int
if_test_cg(void)
{
        int a;
        int b;
        int c;
        if (1 & g)
                return 1;
        return 0;
}

int
if_test_ag(void)
{
        int a;
        int b;
        int c;
        if (a & g)
                return 1;
        return 0;
}

int
if_test_ga(void)
{
        int a;
        int b;
        int c;
        if (g & a)
                return 1;
        return 0;
}

int
if_test_gh(void)
{
        int a;
        int b;
        int c;
        if (g & h)
                return 1;
        return 0;
}

int
if_test_eq0(void)
{
        int a;
        int b;
        int c;
        if ((g & 1) == 0)
                return 1;
        return 0;
}

int
if_add_ga(void)
{
        int a;
        int b;
        int c;
        if (g + a)
                return 1;
        return 0;
}

int
if_sub_gc(void)
{
        int a;
        int b;
        int c;
        if (g - 5)
                return 1;
        return 0;
}

int
if_and_gh(void)
{
        int a;
        int b;
        int c;
        if (g && h)
                return 1;
        return 0;
}

int
if_or_gh(void)
{
        int a;
        int b;
        int c;
        if (g || h)
                return 1;
        return 0;
}

int
while_g(void)
{
        int a;
        int b;
        int c;
        while (g)
                g = g - 1;
        return 0;
}

int
while_lt(void)
{
        int a;
        int b;
        int c;
        while (g < 10)
                g = g + 1;
        return g;
}

int
do_g(void)
{
        int a;
        int b;
        int c;
        do
                g = g - 1;
        while (g);
        return 0;
}

int
for_g(void)
{
        int a;
        int b;
        int c;
        for (g = 0; g < 10; g = g + 1)
                a = a + g;
        return a;
}

int
arg_g(void)
{
        int a;
        int b;
        int c;
        return k(g);
}

int
arg_expr(void)
{
        int a;
        int b;
        int c;
        return k(g + 1);
}

int
arg_two(void)
{
        int a;
        int b;
        int c;
        return k2(a, g);
}

int
arg_gh(void)
{
        int a;
        int b;
        int c;
        return k2(g, h);
}

int
call_store(void)
{
        int a;
        int b;
        int c;
        g = k(a);
        return 0;
}

int
call_left(void)
{
        int a;
        int b;
        int c;
        return g + k(a);
}

int
call_right(void)
{
        int a;
        int b;
        int c;
        return k(a) + g;
}

int
call_sub(void)
{
        int a;
        int b;
        int c;
        return g - k(a);
}

int
call_div(void)
{
        int a;
        int b;
        int c;
        return g / k(a);
}

int
call_cmp(void)
{
        int a;
        int b;
        int c;
        return g < k(a);
}

int
call_if(void)
{
        int a;
        int b;
        int c;
        if (k(g))
                return 1;
        return 0;
}

int
call_store_expr(void)
{
        int a;
        int b;
        int c;
        g = k(a) + b;
        return 0;
}

int
call_store_arg(void)
{
        int a;
        int b;
        int c;
        g = k(g);
        return 0;
}

int
div_gh_ret(void)
{
        int a;
        int b;
        int c;
        return g / h;
}

int
div_ga_ret(void)
{
        int a;
        int b;
        int c;
        return g / a;
}

int
div_ag_ret(void)
{
        int a;
        int b;
        int c;
        return a / g;
}

int
rem_gh_ret(void)
{
        int a;
        int b;
        int c;
        return g % h;
}

int
rem_gh_plus(void)
{
        int a;
        int b;
        int c;
        return g % h + a;
}

int
add_div_gh(void)
{
        int a;
        int b;
        int c;
        return a + g / h;
}

int
add_div_ab(void)
{
        int a;
        int b;
        int c;
        return g + a / b;
}

int
add_rem_gh(void)
{
        int a;
        int b;
        int c;
        return a + g % h;
}

int
div_sums(void)
{
        int a;
        int b;
        int c;
        return (a + g) / (b + h);
}

int
mul_div(void)
{
        int a;
        int b;
        int c;
        return g * (a / h);
}

int
shl_ga_ret(void)
{
        int a;
        int b;
        int c;
        return g << a;
}

int
shl_ag_ret(void)
{
        int a;
        int b;
        int c;
        return a << g;
}

int
shl_gh_ret(void)
{
        int a;
        int b;
        int c;
        return g << h;
}

int
add_shl_bg(void)
{
        int a;
        int b;
        int c;
        return a + (b << g);
}

int
add_shl_ab(void)
{
        int a;
        int b;
        int c;
        return g + (a << b);
}

int
add_shl_gh(void)
{
        int a;
        int b;
        int c;
        return a + (g << h);
}

int
st_div_ab(void)
{
        int a;
        int b;
        int c;
        g = a / b;
        return 0;
}

int
st_div_ha(void)
{
        int a;
        int b;
        int c;
        g = h / a;
        return 0;
}

int
st_div_ah(void)
{
        int a;
        int b;
        int c;
        g = a / h;
        return 0;
}

int
st_rem_ab(void)
{
        int a;
        int b;
        int c;
        g = a % b;
        return 0;
}

int
st_shl_ab(void)
{
        int a;
        int b;
        int c;
        g = a << b;
        return 0;
}

int
st_shl_ah(void)
{
        int a;
        int b;
        int c;
        g = a << h;
        return 0;
}

int
st_add_div(void)
{
        int a;
        int b;
        int c;
        g = a + b / c;
        return 0;
}

int
st_call(void)
{
        int a;
        int b;
        int c;
        g = k(a);
        return 0;
}

int
st_call_h(void)
{
        int a;
        int b;
        int c;
        g = k(h);
        return 0;
}

int
st_call_plus(void)
{
        int a;
        int b;
        int c;
        g = k(a) + b;
        return 0;
}

int
st_plus_call(void)
{
        int a;
        int b;
        int c;
        g = a + k(b);
        return 0;
}

int
st_call_chain(void)
{
        int a;
        int b;
        int c;
        g = h = k(a);
        return 0;
}

int
st_and(void)
{
        int a;
        int b;
        int c;
        g = a + (b && c);
        return 0;
}

int
st_and_left(void)
{
        int a;
        int b;
        int c;
        g = (a && b) + c;
        return 0;
}

int
st_or_h(void)
{
        int a;
        int b;
        int c;
        g = a + (b || h);
        return 0;
}

int
st_lt(void)
{
        int a;
        int b;
        int c;
        g = a + (b < c);
        return 0;
}

int
ret_st_call(void)
{
        int a;
        int b;
        int c;
        return g = k(a);
}

int
ret_st_and(void)
{
        int a;
        int b;
        int c;
        return g = a + (b && c);
}

int
held_call(void)
{
        int a;
        int b;
        int c;
        return g + k(h);
}

int
held_call2(void)
{
        int a;
        int b;
        int c;
        return k(a) + k(g);
}

int
held_call_mul(void)
{
        int a;
        int b;
        int c;
        h = g + k(a) * b;
        return 0;
}

int
held_and(void)
{
        int a;
        int b;
        int c;
        return g + (a && b);
}

int
held_and2(void)
{
        int a;
        int b;
        int c;
        return (g && a) + h;
}

int
held_cmp_call(void)
{
        int a;
        int b;
        int c;
        if (g < k(a))
                return 1;
        return 0;
}

int
chain9(void)
{
        int a;
        int b;
        int c;
        return g + (g + (g + (g + (g + (g + (g + (g + (g + g))))))));
}

int
chain10(void)
{
        int a;
        int b;
        int c;
        return g + (g + (g + (g + (g + (g + (g + (g + (g + (g + g)))))))));
}

int
chain15(void)
{
        int a;
        int b;
        int c;
        return g +
               (g +
                (g +
                 (g +
                  (g +
                   (g +
                    (g +
                     (g + (g + (g + (g + (g + (g + (g + (g + g))))))))))))));
}

int
chain16(void)
{
        int a;
        int b;
        int c;
        return g +
               (g +
                (g +
                 (g +
                  (g +
                   (g +
                    (g +
                     (g +
                      (g + (g + (g + (g + (g + (g + (g + (g + g)))))))))))))));
}

int
chain10_mix(void)
{
        int a;
        int b;
        int c;
        return a + (g + (b + (h + (c + (g + (a + (h + (b + (g + c)))))))));
}

int
st_chain10(void)
{
        int a;
        int b;
        int c;
        h = g + (g + (g + (g + (g + (g + (g + (g + (g + (g + g)))))))));
        return 0;
}
