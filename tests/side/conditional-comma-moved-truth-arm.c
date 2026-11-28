/* A comma over a ?: of two truth values, widened to long as the right
   operand of %: the join holds the second arm's value, which a mov
   brought there on aarch64, valid to 32 bits as the mov wrote it, so
   the widening writes sxtw x1, w1 where the arms' summary says 64.
   Reduced from a random run. */
signed int g0(signed,
              unsigned char,
              int signed,
              long,
              long long int,
              unsigned char,
              long long unsigned,
              int signed,
              char unsigned);
signed int o0;
char o1 = (((('\200' - 32768) && (4097 % 65536)) != (long int) (32767)) *
           (unsigned) (((-1 / 5LL) / 32767)));
extern long signed long o3;
extern int long unsigned o4;
unsigned short
f(long signed long p0,
  char signed p1,
  signed short p2,
  unsigned int p3,
  signed char p4,
  unsigned long long int p5)
{
        unsigned char a = 5ul;
        int long unsigned b = '\x7f';
        short unsigned c = 7;
        long long int d = 32u;
        int t = 7;
        static int unsigned short s2;
        return ((p0 ^ ((a && '\xff') >> t)) %
                (t, ((short unsigned) (o0) ?
                         ((0xffffffff % s2) ==
                          ((o4 == 70000) ? (o1 < '\0') : 255)) :
                         (~t >= (signed int) (short int) (long long) s2))));
}
int o0 = (((5llu || 0xffffffffffffffff) % -128) >>
          (-129 ^ (32 + 9223372036854775808u)));
unsigned extern o2;
long long extern o3 = 1;
unsigned long int extern o4 = (32u && 0xffffffffffffffff);
