/* An object of each integer type defined at file scope, read at its
   type, widened, stored, assigned as a value, an operand of each
   family of operator and of a comparison, an argument, under a
   conversion: each access the label's, an extending load of its own
   sign, movsbl gc(%rip), %eax, ldrb w0, [x16, :lo12:gc] for plain char
   on aarch64, and the extending load to 64 bits of a signed one,
   movslq gi(%rip), %rax and ldrsw x0, [x16, :lo12:gi]. */
int k(int x);
char gc;
signed char gsc;
unsigned char guc;
short gs;
unsigned short gus;
int gi;
unsigned gu;
long gl;
unsigned long gul;
long long gll;
unsigned long long gull;

char
read_c(void)
{
        return gc;
}

long
widened_c(void)
{
        return gc;
}

unsigned long
widened_unsigned_c(void)
{
        return gc;
}

int
stores_c(char a)
{
        gc = 5;
        gc = 0;
        gc = -1;
        gc = 100000;
        gc = 0x123456789;
        gc = a;
        return 0;
}

char
assigned_c(char a)
{
        return gc = a;
}

long
sum_c(long a)
{
        return a + gc;
}

int
compared_c(char a)
{
        if (gc)
                return 1;
        if (gc < a)
                return 2;
        if (gc == 5)
                return 3;
        return gc > 7;
}

int
operators_c(int a)
{
        return gc * 5 + a / gc - (gc & 1) + (gc << a) + (a % gc);
}

int
argument_c(void)
{
        gc = k(gc);
        return k(gc + 1);
}

long
casts_c(void)
{
        return (long) (int) gc + (unsigned char) gc + (short) (int) gc;
}

signed char
read_sc(void)
{
        return gsc;
}

long
widened_sc(void)
{
        return gsc;
}

unsigned long
widened_unsigned_sc(void)
{
        return gsc;
}

int
stores_sc(signed char a)
{
        gsc = 5;
        gsc = 0;
        gsc = -1;
        gsc = 100000;
        gsc = 0x123456789;
        gsc = a;
        return 0;
}

signed char
assigned_sc(signed char a)
{
        return gsc = a;
}

long
sum_sc(long a)
{
        return a + gsc;
}

int
compared_sc(signed char a)
{
        if (gsc)
                return 1;
        if (gsc < a)
                return 2;
        if (gsc == 5)
                return 3;
        return gsc > 7;
}

int
operators_sc(int a)
{
        return gsc * 5 + a / gsc - (gsc & 1) + (gsc << a) + (a % gsc);
}

int
argument_sc(void)
{
        gsc = k(gsc);
        return k(gsc + 1);
}

long
casts_sc(void)
{
        return (long) (int) gsc + (unsigned char) gsc + (short) (int) gsc;
}

unsigned char
read_uc(void)
{
        return guc;
}

long
widened_uc(void)
{
        return guc;
}

unsigned long
widened_unsigned_uc(void)
{
        return guc;
}

int
stores_uc(unsigned char a)
{
        guc = 5;
        guc = 0;
        guc = -1;
        guc = 100000;
        guc = 0x123456789;
        guc = a;
        return 0;
}

unsigned char
assigned_uc(unsigned char a)
{
        return guc = a;
}

long
sum_uc(long a)
{
        return a + guc;
}

int
compared_uc(unsigned char a)
{
        if (guc)
                return 1;
        if (guc < a)
                return 2;
        if (guc == 5)
                return 3;
        return guc > 7;
}

int
operators_uc(int a)
{
        return guc * 5 + a / guc - (guc & 1) + (guc << a) + (a % guc);
}

int
argument_uc(void)
{
        guc = k(guc);
        return k(guc + 1);
}

long
casts_uc(void)
{
        return (long) (int) guc + (unsigned char) guc + (short) (int) guc;
}

short
read_s(void)
{
        return gs;
}

long
widened_s(void)
{
        return gs;
}

unsigned long
widened_unsigned_s(void)
{
        return gs;
}

int
stores_s(short a)
{
        gs = 5;
        gs = 0;
        gs = -1;
        gs = 100000;
        gs = 0x123456789;
        gs = a;
        return 0;
}

short
assigned_s(short a)
{
        return gs = a;
}

long
sum_s(long a)
{
        return a + gs;
}

int
compared_s(short a)
{
        if (gs)
                return 1;
        if (gs < a)
                return 2;
        if (gs == 5)
                return 3;
        return gs > 7;
}

int
operators_s(int a)
{
        return gs * 5 + a / gs - (gs & 1) + (gs << a) + (a % gs);
}

int
argument_s(void)
{
        gs = k(gs);
        return k(gs + 1);
}

long
casts_s(void)
{
        return (long) (int) gs + (unsigned char) gs + (short) (int) gs;
}

unsigned short
read_us(void)
{
        return gus;
}

long
widened_us(void)
{
        return gus;
}

unsigned long
widened_unsigned_us(void)
{
        return gus;
}

int
stores_us(unsigned short a)
{
        gus = 5;
        gus = 0;
        gus = -1;
        gus = 100000;
        gus = 0x123456789;
        gus = a;
        return 0;
}

unsigned short
assigned_us(unsigned short a)
{
        return gus = a;
}

long
sum_us(long a)
{
        return a + gus;
}

int
compared_us(unsigned short a)
{
        if (gus)
                return 1;
        if (gus < a)
                return 2;
        if (gus == 5)
                return 3;
        return gus > 7;
}

int
operators_us(int a)
{
        return gus * 5 + a / gus - (gus & 1) + (gus << a) + (a % gus);
}

int
argument_us(void)
{
        gus = k(gus);
        return k(gus + 1);
}

long
casts_us(void)
{
        return (long) (int) gus + (unsigned char) gus + (short) (int) gus;
}

int
read_i(void)
{
        return gi;
}

long
widened_i(void)
{
        return gi;
}

unsigned long
widened_unsigned_i(void)
{
        return gi;
}

int
stores_i(int a)
{
        gi = 5;
        gi = 0;
        gi = -1;
        gi = 100000;
        gi = 0x123456789;
        gi = a;
        return 0;
}

int
assigned_i(int a)
{
        return gi = a;
}

long
sum_i(long a)
{
        return a + gi;
}

int
compared_i(int a)
{
        if (gi)
                return 1;
        if (gi < a)
                return 2;
        if (gi == 5)
                return 3;
        return gi > 7;
}

int
operators_i(int a)
{
        return gi * 5 + a / gi - (gi & 1) + (gi << a) + (a % gi);
}

int
argument_i(void)
{
        gi = k(gi);
        return k(gi + 1);
}

long
casts_i(void)
{
        return (long) (int) gi + (unsigned char) gi + (short) (int) gi;
}

unsigned
read_u(void)
{
        return gu;
}

long
widened_u(void)
{
        return gu;
}

unsigned long
widened_unsigned_u(void)
{
        return gu;
}

int
stores_u(unsigned a)
{
        gu = 5;
        gu = 0;
        gu = -1;
        gu = 100000;
        gu = 0x123456789;
        gu = a;
        return 0;
}

unsigned
assigned_u(unsigned a)
{
        return gu = a;
}

long
sum_u(long a)
{
        return a + gu;
}

int
compared_u(unsigned a)
{
        if (gu)
                return 1;
        if (gu < a)
                return 2;
        if (gu == 5)
                return 3;
        return gu > 7;
}

int
operators_u(int a)
{
        return gu * 5 + a / gu - (gu & 1) + (gu << a) + (a % gu);
}

int
argument_u(void)
{
        gu = k(gu);
        return k(gu + 1);
}

long
casts_u(void)
{
        return (long) (int) gu + (unsigned char) gu + (short) (int) gu;
}

long
read_l(void)
{
        return gl;
}

long
widened_l(void)
{
        return gl;
}

unsigned long
widened_unsigned_l(void)
{
        return gl;
}

int
stores_l(long a)
{
        gl = 5;
        gl = 0;
        gl = -1;
        gl = 100000;
        gl = 0x123456789;
        gl = a;
        return 0;
}

long
assigned_l(long a)
{
        return gl = a;
}

long
sum_l(long a)
{
        return a + gl;
}

int
compared_l(long a)
{
        if (gl)
                return 1;
        if (gl < a)
                return 2;
        if (gl == 5)
                return 3;
        return gl > 7;
}

int
operators_l(int a)
{
        return gl * 5 + a / gl - (gl & 1) + (gl << a) + (a % gl);
}

int
argument_l(void)
{
        gl = k(gl);
        return k(gl + 1);
}

long
casts_l(void)
{
        return (long) (int) gl + (unsigned char) gl + (short) (int) gl;
}

unsigned long
read_ul(void)
{
        return gul;
}

long
widened_ul(void)
{
        return gul;
}

unsigned long
widened_unsigned_ul(void)
{
        return gul;
}

int
stores_ul(unsigned long a)
{
        gul = 5;
        gul = 0;
        gul = -1;
        gul = 100000;
        gul = 0x123456789;
        gul = a;
        return 0;
}

unsigned long
assigned_ul(unsigned long a)
{
        return gul = a;
}

long
sum_ul(long a)
{
        return a + gul;
}

int
compared_ul(unsigned long a)
{
        if (gul)
                return 1;
        if (gul < a)
                return 2;
        if (gul == 5)
                return 3;
        return gul > 7;
}

int
operators_ul(int a)
{
        return gul * 5 + a / gul - (gul & 1) + (gul << a) + (a % gul);
}

int
argument_ul(void)
{
        gul = k(gul);
        return k(gul + 1);
}

long
casts_ul(void)
{
        return (long) (int) gul + (unsigned char) gul + (short) (int) gul;
}

long long
read_ll(void)
{
        return gll;
}

long
widened_ll(void)
{
        return gll;
}

unsigned long
widened_unsigned_ll(void)
{
        return gll;
}

int
stores_ll(long long a)
{
        gll = 5;
        gll = 0;
        gll = -1;
        gll = 100000;
        gll = 0x123456789;
        gll = a;
        return 0;
}

long long
assigned_ll(long long a)
{
        return gll = a;
}

long
sum_ll(long a)
{
        return a + gll;
}

int
compared_ll(long long a)
{
        if (gll)
                return 1;
        if (gll < a)
                return 2;
        if (gll == 5)
                return 3;
        return gll > 7;
}

int
operators_ll(int a)
{
        return gll * 5 + a / gll - (gll & 1) + (gll << a) + (a % gll);
}

int
argument_ll(void)
{
        gll = k(gll);
        return k(gll + 1);
}

long
casts_ll(void)
{
        return (long) (int) gll + (unsigned char) gll + (short) (int) gll;
}

unsigned long long
read_ull(void)
{
        return gull;
}

long
widened_ull(void)
{
        return gull;
}

unsigned long
widened_unsigned_ull(void)
{
        return gull;
}

int
stores_ull(unsigned long long a)
{
        gull = 5;
        gull = 0;
        gull = -1;
        gull = 100000;
        gull = 0x123456789;
        gull = a;
        return 0;
}

unsigned long long
assigned_ull(unsigned long long a)
{
        return gull = a;
}

long
sum_ull(long a)
{
        return a + gull;
}

int
compared_ull(unsigned long long a)
{
        if (gull)
                return 1;
        if (gull < a)
                return 2;
        if (gull == 5)
                return 3;
        return gull > 7;
}

int
operators_ull(int a)
{
        return gull * 5 + a / gull - (gull & 1) + (gull << a) + (a % gull);
}

int
argument_ull(void)
{
        gull = k(gull);
        return k(gull + 1);
}

long
casts_ull(void)
{
        return (long) (int) gull + (unsigned char) gull + (short) (int) gull;
}
