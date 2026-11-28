/* switch on an object of static storage of every integer type, the
   shapes of switch-types.c with the object switched on at file scope:
   defined, its place a label, movsbl d_dir_c(%rip), %eax; only
   declared, through the address the GOT holds.  Each function is its
   shape's in switch-types.c, named defined_ or declared_ and the shape,
   and its object d_ or e_ and the shape. */

unsigned char d_e_dup_uc_m1 = 5;
extern unsigned char e_e_dup_uc_m1;
char d_range_c_300 = 5;
extern char e_range_c_300;
int d_range_i_big = 5;
extern int e_range_i_big;
unsigned d_range_u_m1 = 5;
extern unsigned e_range_u_m1;
unsigned long d_range_ul_m1 = 5;
extern unsigned long e_range_ul_m1;
int d_dirbig_i = 5;
extern int e_dirbig_i;
long d_dirbig_l = 5;
extern long e_dirbig_l;
int d_span_i = 5;
extern int e_span_i;
unsigned d_span_u = 5;
extern unsigned e_span_u;
int d_simp_i = 5;
extern int e_simp_i;
unsigned d_simp_u = 5;
extern unsigned e_simp_u;
char d_dir_c = 5;
extern char e_dir_c;
long d_dir_l = 5;
extern long e_dir_l;
char d_dir0_c = 5;
extern char e_dir0_c;
unsigned d_dir0_u = 5;
extern unsigned e_dir0_u;
long d_dir0_l = 5;
extern long e_dir0_l;
unsigned char d_dir0_uc = 5;
extern unsigned char e_dir0_uc;
unsigned d_dirneg_u = 5;
extern unsigned e_dirneg_u;
long d_dirhi_l = 5;
extern long e_dirhi_l;
long d_dirmin_l = 5;
extern long e_dirmin_l;
long d_simpbig_l = 5;
extern long e_simpbig_l;
unsigned d_simpbig_u = 5;
extern unsigned e_simpbig_u;
int d_xrem_dir0_i = 5;
extern int e_xrem_dir0_i;
unsigned d_xrem_dir0_u = 5;
extern unsigned e_xrem_dir0_u;
long d_xrem_dir_l = 5;
extern long e_xrem_dir_l;
char d_xpost_dir0_c = 5;
extern char e_xpost_dir0_c;
unsigned char d_xcast_dir0_uc = 5;
extern unsigned char e_xcast_dir0_uc;
short d_xcastl_dir0_s = 5;
extern short e_xcastl_dir0_s;
char d_xcmpd_dir0_c = 5;
extern char e_xcmpd_dir0_c;
long d_xrem_hash_l = 5;
extern long e_xrem_hash_l;
long d_xpost_hash_l = 5;
extern long e_xpost_hash_l;
int d_hashneg_i = 5;
extern int e_hashneg_i;
unsigned d_hashneg_u = 5;
extern unsigned e_hashneg_u;
long d_hashneg_l = 5;
extern long e_hashneg_l;
unsigned long d_hashneg_ul = 5;
extern unsigned long e_hashneg_ul;
int d_hashw_i = 5;
extern int e_hashw_i;
long d_hashw_l = 5;
extern long e_hashw_l;

int
defined_e_dup_uc_m1(void)
{
        switch (d_e_dup_uc_m1) {
        case -1: return 1;
        case 255: return 2;
        }
        return 0;
}

int
declared_e_dup_uc_m1(void)
{
        switch (e_e_dup_uc_m1) {
        case -1: return 1;
        case 255: return 2;
        }
        return 0;
}

int
defined_range_c_300(void)
{
        switch (d_range_c_300) {
        case 300: return 1;
        case 255: return 2;
        case -1: return 3;
        }
        return 0;
}

int
declared_range_c_300(void)
{
        switch (e_range_c_300) {
        case 300: return 1;
        case 255: return 2;
        case -1: return 3;
        }
        return 0;
}

int
defined_range_i_big(void)
{
        switch (d_range_i_big) {
        case 0x100000001: return 1;
        case 0x7fffffff: return 2;
        case 0x80000000: return 3;
        }
        return 0;
}

int
declared_range_i_big(void)
{
        switch (e_range_i_big) {
        case 0x100000001: return 1;
        case 0x7fffffff: return 2;
        case 0x80000000: return 3;
        }
        return 0;
}

int
defined_range_u_m1(void)
{
        switch (d_range_u_m1) {
        case -1: return 1;
        case 0x100000000: return 2;
        }
        return 0;
}

int
declared_range_u_m1(void)
{
        switch (e_range_u_m1) {
        case -1: return 1;
        case 0x100000000: return 2;
        }
        return 0;
}

int
defined_range_ul_m1(void)
{
        switch (d_range_ul_m1) {
        case -1: return 1;
        case -2: return 2;
        case 0: return 3;
        }
        return 0;
}

int
declared_range_ul_m1(void)
{
        switch (e_range_ul_m1) {
        case -1: return 1;
        case -2: return 2;
        case 0: return 3;
        }
        return 0;
}

int
defined_dirbig_i(void)
{
        switch (d_dirbig_i) {
        case 0x100000000: return 1;
        case 0x100000001: return 2;
        case 0x100000002: return 3;
        case 0x100000004: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dirbig_i(void)
{
        switch (e_dirbig_i) {
        case 0x100000000: return 1;
        case 0x100000001: return 2;
        case 0x100000002: return 3;
        case 0x100000004: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dirbig_l(void)
{
        switch (d_dirbig_l) {
        case 0x100000000: return 1;
        case 0x100000001: return 2;
        case 0x100000002: return 3;
        case 0x100000004: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dirbig_l(void)
{
        switch (e_dirbig_l) {
        case 0x100000000: return 1;
        case 0x100000001: return 2;
        case 0x100000002: return 3;
        case 0x100000004: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_span_i(void)
{
        switch (d_span_i) {
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_span_i(void)
{
        switch (e_span_i) {
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_span_u(void)
{
        switch (d_span_u) {
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_span_u(void)
{
        switch (e_span_u) {
        case -1: return 1;
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_simp_i(void)
{
        switch (d_simp_i) {
        case 10: return 1;
        case 0: return 2;
        case -1: return 3;
        case 100: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_simp_i(void)
{
        switch (e_simp_i) {
        case 10: return 1;
        case 0: return 2;
        case -1: return 3;
        case 100: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_simp_u(void)
{
        switch (d_simp_u) {
        case 10: return 1;
        case 0: return 2;
        case -1: return 3;
        case 100: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_simp_u(void)
{
        switch (e_simp_u) {
        case 10: return 1;
        case 0: return 2;
        case -1: return 3;
        case 100: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dir_c(void)
{
        switch (d_dir_c) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dir_c(void)
{
        switch (e_dir_c) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dir_l(void)
{
        switch (d_dir_l) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dir_l(void)
{
        switch (e_dir_l) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dir0_c(void)
{
        switch (d_dir0_c) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dir0_c(void)
{
        switch (e_dir0_c) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dir0_u(void)
{
        switch (d_dir0_u) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dir0_u(void)
{
        switch (e_dir0_u) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dir0_l(void)
{
        switch (d_dir0_l) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dir0_l(void)
{
        switch (e_dir0_l) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dir0_uc(void)
{
        switch (d_dir0_uc) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dir0_uc(void)
{
        switch (e_dir0_uc) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dirneg_u(void)
{
        switch (d_dirneg_u) {
        case -1: return 1;
        case -2: return 2;
        case -3: return 3;
        case -5: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_dirneg_u(void)
{
        switch (e_dirneg_u) {
        case -1: return 1;
        case -2: return 2;
        case -3: return 3;
        case -5: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_dirhi_l(void)
{
        switch (d_dirhi_l) {
        case 0xfffffffe: return 1;
        case 0xffffffff: return 2;
        case 0xfffffffd: return 3;
        default: return 99;
        }
        return 0;
}

int
declared_dirhi_l(void)
{
        switch (e_dirhi_l) {
        case 0xfffffffe: return 1;
        case 0xffffffff: return 2;
        case 0xfffffffd: return 3;
        default: return 99;
        }
        return 0;
}

int
defined_dirmin_l(void)
{
        switch (d_dirmin_l) {
        case -0x7fffffffffffffff - 1: return 1;
        case -0x7fffffffffffffff: return 2;
        case -0x7ffffffffffffffe: return 3;
        default: return 99;
        }
        return 0;
}

int
declared_dirmin_l(void)
{
        switch (e_dirmin_l) {
        case -0x7fffffffffffffff - 1: return 1;
        case -0x7fffffffffffffff: return 2;
        case -0x7ffffffffffffffe: return 3;
        default: return 99;
        }
        return 0;
}

int
defined_simpbig_l(void)
{
        switch (d_simpbig_l) {
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
declared_simpbig_l(void)
{
        switch (e_simpbig_l) {
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
defined_simpbig_u(void)
{
        switch (d_simpbig_u) {
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
declared_simpbig_u(void)
{
        switch (e_simpbig_u) {
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
defined_xrem_dir0_i(void)
{
        int y;
        int i;
        long l;
        switch (d_xrem_dir0_i % y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_xrem_dir0_i(void)
{
        int y;
        int i;
        long l;
        switch (e_xrem_dir0_i % y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_xrem_dir0_u(void)
{
        unsigned y;
        int i;
        long l;
        switch (d_xrem_dir0_u % y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_xrem_dir0_u(void)
{
        unsigned y;
        int i;
        long l;
        switch (e_xrem_dir0_u % y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_xrem_dir_l(void)
{
        long y;
        int i;
        long l;
        switch (d_xrem_dir_l % y) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_xrem_dir_l(void)
{
        long y;
        int i;
        long l;
        switch (e_xrem_dir_l % y) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 5: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_xpost_dir0_c(void)
{
        char y;
        int i;
        long l;
        switch (d_xpost_dir0_c++) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_xpost_dir0_c(void)
{
        char y;
        int i;
        long l;
        switch (e_xpost_dir0_c++) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_xcast_dir0_uc(void)
{
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
declared_xcast_dir0_uc(void)
{
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
defined_xcastl_dir0_s(void)
{
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
declared_xcastl_dir0_s(void)
{
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
defined_xcmpd_dir0_c(void)
{
        char y;
        int i;
        long l;
        switch (d_xcmpd_dir0_c += y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
declared_xcmpd_dir0_c(void)
{
        char y;
        int i;
        long l;
        switch (e_xcmpd_dir0_c += y) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 4: return 4;
        default: return 99;
        }
        return 0;
}

int
defined_xrem_hash_l(void)
{
        long y;
        int i;
        long l;
        switch (d_xrem_hash_l % y) {
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
declared_xrem_hash_l(void)
{
        long y;
        int i;
        long l;
        switch (e_xrem_hash_l % y) {
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
defined_xpost_hash_l(void)
{
        long y;
        int i;
        long l;
        switch (d_xpost_hash_l++) {
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
declared_xpost_hash_l(void)
{
        long y;
        int i;
        long l;
        switch (e_xpost_hash_l++) {
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
defined_hashneg_i(void)
{
        switch (d_hashneg_i) {
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
declared_hashneg_i(void)
{
        switch (e_hashneg_i) {
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
defined_hashneg_u(void)
{
        switch (d_hashneg_u) {
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
declared_hashneg_u(void)
{
        switch (e_hashneg_u) {
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
defined_hashneg_l(void)
{
        switch (d_hashneg_l) {
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
declared_hashneg_l(void)
{
        switch (e_hashneg_l) {
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
defined_hashneg_ul(void)
{
        switch (d_hashneg_ul) {
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
declared_hashneg_ul(void)
{
        switch (e_hashneg_ul) {
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
defined_hashw_i(void)
{
        switch (d_hashw_i) {
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
declared_hashw_i(void)
{
        switch (e_hashw_i) {
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
defined_hashw_l(void)
{
        switch (d_hashw_l) {
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
declared_hashw_l(void)
{
        switch (e_hashw_l) {
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
