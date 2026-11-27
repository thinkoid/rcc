/* Initializers of the other integer types: a constant converted to the
   object's type, (char) 300 being 44, an unsigned -1 its type's every
   bit; a 64-bit sum past long's range warned of and wrapped; an
   unsigned sum that wraps, which is no overflow, 6.2.5p11; and a plain
   char conversion, -56 on x86-64 and 200 on aarch64. */
char c1 = 300;
unsigned u1 = -1;
long l1 = 2147483648;
unsigned long ul1 = -1;
long l2 = 9223372036854775807 + 1;
unsigned u2 = 4294967295u + 1;
int i1 = (char) 200;
short s1 = 70000;
unsigned char uc1 = -1;
long l3 = (long) 2147483647 + 1;
int i2 = 2147483647L + 1;
