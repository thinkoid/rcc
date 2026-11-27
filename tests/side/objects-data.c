/* The data of an object of each integer type, rcc's object(): its
   .globl, .type and .size, its alignment, an integer's its size and no
   .p2align for a byte, its label, then its value in the directive of
   its size, .byte, .short, .long or .quad, signed for a signed type and
   unsigned for an unsigned one, in .data; or its zeros, .zero, in .bss
   for one without an initializer.  A value past the type's range is its
   low bits, 6.3.1.3; plain char is signed on x86-64 and unsigned on
   aarch64, so its -1 is .byte -1 on one and .byte 255 on the other. */
char c_negative = -1;
char c_zero = 0;
char c_past = 200;
char c_none;
signed char sc_negative = -1;
signed char sc_zero = 0;
signed char sc_past = 200;
signed char sc_none;
unsigned char uc_negative = -1;
unsigned char uc_zero = 0;
unsigned char uc_past = 300;
unsigned char uc_none;
short s_negative = -1;
short s_zero = 0;
short s_past = 70000;
short s_none;
unsigned short us_negative = -1;
unsigned short us_zero = 0;
unsigned short us_past = 70000;
unsigned short us_none;
int i_negative = -1;
int i_zero = 0;
int i_past = 4294967297;
int i_none;
unsigned u_negative = -1;
unsigned u_zero = 0;
unsigned u_past = 4294967297;
unsigned u_none;
long l_negative = -1;
long l_zero = 0;
long l_past = 18446744073709551615u;
long l_none;
unsigned long ul_negative = -1;
unsigned long ul_zero = 0;
unsigned long ul_past = 0x8000000000000000;
unsigned long ul_none;
long long ll_negative = -1;
long long ll_zero = 0;
long long ll_past = 0xffffffffffffffff;
long long ll_none;
unsigned long long ull_negative = -1;
unsigned long long ull_zero = 0;
unsigned long long ull_past = -9223372036854775807 - 1;
unsigned long long ull_none;
