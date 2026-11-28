/* switch on a controlling expression of every integer type, 6.8.5.3:
   the expression promoted, each label converted to the promoted type,
   so case 0x100000001: on an int is the case 1, and the rule of the
   Seventh Edition compiler reads the type's values: they are sorted in
   its order, -1 first on an int and last on an unsigned, the range is
   modulo 2^N at its width and the hash reads the values unsigned at it.
   Every instruction of the dispatch is at the promoted width, cmpq $4,
   %rax, subq, divq, and a label no 32-bit immediate holds is movabsq
   into rcx, mov and movk on aarch64; the bounds check is unsigned
   whatever the type.  x86-64 extends a 32-bit index with movl %eax,
   %eax where no write of an unsigned value at 32 bits left it valid to
   64; aarch64 reads it uxtw #3, and a 64-bit one lsl #3.

   Each function is one shape, its name the shape: the form, none one
   dir dir0 simp hash hashneg dirbig dirmin simpbig hashbig dirneg span
   dirhi, as the labels' values reach it, an x and an expression of the
   object before it, xrem a remainder, xpost a postfix step, xcast a
   cast, xcmpd a compound assignment, nd no default, range_ and hashw_
   others; then the type, c sc uc s us i u l ul for char to unsigned
   long. */


int
e_dup_uc_m1(void)
{
        unsigned char x;
        switch (x) {
        case -1: return 1;
        case 255: return 2;
        }
        return 0;
}

int
range_c_300(void)
{
        char x;
        switch (x) {
        case 300: return 1;
        case 255: return 2;
        case -1: return 3;
        }
        return 0;
}

int
range_i_big(void)
{
        int x;
        switch (x) {
        case 0x100000001: return 1;
        case 0x7fffffff: return 2;
        case 0x80000000: return 3;
        }
        return 0;
}

int
range_u_m1(void)
{
        unsigned x;
        switch (x) {
        case -1: return 1;
        case 0x100000000: return 2;
        }
        return 0;
}

int
range_ul_m1(void)
{
        unsigned long x;
        switch (x) {
        case -1: return 1;
        case -2: return 2;
        case 0: return 3;
        }
        return 0;
}

int
dirbig_i(void)
{
        int x;
        switch (x) {
        case 0x100000000: return 1;
        case 0x100000001: return 2;
        case 0x100000002: return 3;
        case 0x100000004: return 4;
        default: return 99;
        }
        return 0;
}

int
dirbig_l(void)
{
        long x;
        switch (x) {
        case 0x100000000: return 1;
        case 0x100000001: return 2;
        case 0x100000002: return 3;
        case 0x100000004: return 4;
        default: return 99;
        }
        return 0;
}

int
span_i(void)
{
        int x;
        switch (x) {
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 99;
        }
        return 0;
}

int
span_u(void)
{
        unsigned x;
        switch (x) {
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 99;
        }
        return 0;
}

int
simp_i(void)
{
        int x;
        switch (x) {
        case 10: return 1;
        case 0: return 2;
        case -1: return 3;
        case 100: return 4;
        default: return 99;
        }
        return 0;
}

int
simp_u(void)
{
        unsigned x;
        switch (x) {
        case 10: return 1;
        case 0: return 2;
        case -1: return 3;
        case 100: return 4;
        default: return 99;
        }
        return 0;
}

int
dir_c(void)
{
        char x;
        switch (x) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
dir_l(void)
{
        long x;
        switch (x) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
dir0_c(void)
{
        char x;
        switch (x) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
dir0_u(void)
{
        unsigned x;
        switch (x) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
dir0_l(void)
{
        long x;
        switch (x) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
dir0_uc(void)
{
        unsigned char x;
        switch (x) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
dirneg_u(void)
{
        unsigned x;
        switch (x) {
        case -1: return 1;
        case -2: return 2;
        case -3: return 3;
        case -5: return 4;
        default: return 99;
        }
        return 0;
}

int
dirhi_l(void)
{
        long x;
        switch (x) {
        case 0xfffffffe: return 1;
        case 0xffffffff: return 2;
        case 0xfffffffd: return 3;
        default: return 99;
        }
        return 0;
}

int
dirmin_l(void)
{
        long x;
        switch (x) {
        case -0x7fffffffffffffff - 1: return 1;
        case -0x7fffffffffffffff: return 2;
        case -0x7ffffffffffffffe: return 3;
        default: return 99;
        }
        return 0;
}

int
simpbig_l(void)
{
        long x;
        switch (x) {
        case 0x123456789: return 1;
        case 0: return 2;
        case -1: return 3;
        case 0x7fffffff: return 4;
        case 0x80000000: return 5;
        default: return 99;
        }
        return 0;
}

int
simpbig_u(void)
{
        unsigned x;
        switch (x) {
        case 0x123456789: return 1;
        case 0: return 2;
        case -1: return 3;
        case 0x7fffffff: return 4;
        case 0x80000000: return 5;
        default: return 99;
        }
        return 0;
}

int
xrem_dir0_i(void)
{
        int x;
        int y;
        int i;
        long l;
        switch (x % y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
xrem_dir0_u(void)
{
        unsigned x;
        unsigned y;
        int i;
        long l;
        switch (x % y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
xrem_dir_l(void)
{
        long x;
        long y;
        int i;
        long l;
        switch (x % y) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
xpost_dir0_c(void)
{
        char x;
        char y;
        int i;
        long l;
        switch (x++) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
xcast_dir0_uc(void)
{
        unsigned char x;
        unsigned char y;
        int i;
        long l;
        switch ((unsigned char) i) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
xcastl_dir0_s(void)
{
        short x;
        short y;
        int i;
        long l;
        switch ((short) l) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
xcmpd_dir0_c(void)
{
        char x;
        char y;
        int i;
        long l;
        switch (x += y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
xrem_hash_l(void)
{
        long x;
        long y;
        int i;
        long l;
        switch (x % y) {
        case 200: return 1;
        case 201: return 2;
        case 204: return 3;
        case 301: return 4;
        case 302: return 5;
        case 304: return 6;
        case 400: return 7;
        case 401: return 8;
        case 403: return 9;
        case 404: return 10;
        case 500: return 11;
        case 503: return 12;
        default: return 99;
        }
        return 0;
}

int
xpost_hash_l(void)
{
        long x;
        long y;
        int i;
        long l;
        switch (x++) {
        case 200: return 1;
        case 201: return 2;
        case 204: return 3;
        case 301: return 4;
        case 302: return 5;
        case 304: return 6;
        case 400: return 7;
        case 401: return 8;
        case 403: return 9;
        case 404: return 10;
        case 500: return 11;
        case 503: return 12;
        default: return 99;
        }
        return 0;
}

int
hashneg_i(void)
{
        int x;
        switch (x) {
        case -50: return 1;
        case -40: return 2;
        case -30: return 3;
        case -20: return 4;
        case -10: return 5;
        case 0: return 6;
        case 10: return 7;
        case 20: return 8;
        case 30: return 9;
        case 40: return 10;
        default: return 99;
        }
        return 0;
}

int
hashneg_u(void)
{
        unsigned x;
        switch (x) {
        case -50: return 1;
        case -40: return 2;
        case -30: return 3;
        case -20: return 4;
        case -10: return 5;
        case 0: return 6;
        case 10: return 7;
        case 20: return 8;
        case 30: return 9;
        case 40: return 10;
        default: return 99;
        }
        return 0;
}

int
hashneg_l(void)
{
        long x;
        switch (x) {
        case -50: return 1;
        case -40: return 2;
        case -30: return 3;
        case -20: return 4;
        case -10: return 5;
        case 0: return 6;
        case 10: return 7;
        case 20: return 8;
        case 30: return 9;
        case 40: return 10;
        default: return 99;
        }
        return 0;
}

int
hashneg_ul(void)
{
        unsigned long x;
        switch (x) {
        case -50: return 1;
        case -40: return 2;
        case -30: return 3;
        case -20: return 4;
        case -10: return 5;
        case 0: return 6;
        case 10: return 7;
        case 20: return 8;
        case 30: return 9;
        case 40: return 10;
        default: return 99;
        }
        return 0;
}

int
hashw_i(void)
{
        int x;
        switch (x) {
        case -194: return 1;
        case -188: return 2;
        case -164: return 3;
        case -149: return 4;
        case -140: return 5;
        case -125: return 6;
        case -122: return 7;
        case -119: return 8;
        case -98: return 9;
        case -92: return 10;
        case -77: return 11;
        case -71: return 12;
        case -53: return 13;
        case -47: return 14;
        case -38: return 15;
        case 16: return 16;
        case 25: return 17;
        case 34: return 18;
        case 58: return 19;
        case 97: return 20;
        case 100: return 21;
        case 109: return 22;
        case 127: return 23;
        case 133: return 24;
        case 145: return 25;
        case 178: return 26;
        case 181: return 27;
        case 196: return 28;
        default: return 99;
        }
        return 0;
}

int
hashw_l(void)
{
        long x;
        switch (x) {
        case -194: return 1;
        case -188: return 2;
        case -164: return 3;
        case -149: return 4;
        case -140: return 5;
        case -125: return 6;
        case -122: return 7;
        case -119: return 8;
        case -98: return 9;
        case -92: return 10;
        case -77: return 11;
        case -71: return 12;
        case -53: return 13;
        case -47: return 14;
        case -38: return 15;
        case 16: return 16;
        case 25: return 17;
        case 34: return 18;
        case 58: return 19;
        case 97: return 20;
        case 100: return 21;
        case 109: return 22;
        case 127: return 23;
        case 133: return 24;
        case 145: return 25;
        case 178: return 26;
        case 181: return 27;
        case 196: return 28;
        default: return 99;
        }
        return 0;
}
