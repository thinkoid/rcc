/* An argument of each type past the registers.  On x86-64 the seventh:
   a 64-bit slot pushed as it is, pushq -40(%rbp), a narrower one
   through rax after its extending load to 32 bits, movsbl -29(%rbp),
   %eax and pushq %rax.  On aarch64 the ninth, copied to its own 8
   bytes of the outgoing area at its width by the plain load, ldrb
   w17, [x29, #52] and strb w17, [sp], a signed one not extended.
   Each from a local, from -1 and from 0x123456789. */
int seventh_char(int a1, int a2, int a3, int a4, int a5, int a6, char x);
int seventh_signed_char(
    int a1, int a2, int a3, int a4, int a5, int a6, signed char x);
int seventh_unsigned_char(
    int a1, int a2, int a3, int a4, int a5, int a6, unsigned char x);
int seventh_short(int a1, int a2, int a3, int a4, int a5, int a6, short x);
int seventh_unsigned_short(
    int a1, int a2, int a3, int a4, int a5, int a6, unsigned short x);
int seventh_int(int a1, int a2, int a3, int a4, int a5, int a6, int x);
int seventh_long(int a1, int a2, int a3, int a4, int a5, int a6, long x);
int ninth_char(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, char x);
int ninth_signed_char(int a1,
                      int a2,
                      int a3,
                      int a4,
                      int a5,
                      int a6,
                      int a7,
                      int a8,
                      signed char x);
int ninth_unsigned_char(int a1,
                        int a2,
                        int a3,
                        int a4,
                        int a5,
                        int a6,
                        int a7,
                        int a8,
                        unsigned char x);
int ninth_short(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, short x);
int ninth_unsigned_short(int a1,
                         int a2,
                         int a3,
                         int a4,
                         int a5,
                         int a6,
                         int a7,
                         int a8,
                         unsigned short x);
int ninth_int(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int x);
int ninth_long(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, long x);

int
push_char(void)
{
        char p;
        return seventh_char(0, 0, 0, 0, 0, 0, p);
}

int
push_signed_char(void)
{
        signed char p;
        return seventh_signed_char(0, 0, 0, 0, 0, 0, p);
}

int
push_unsigned_char(void)
{
        unsigned char p;
        return seventh_unsigned_char(0, 0, 0, 0, 0, 0, p);
}

int
push_short(void)
{
        short p;
        return seventh_short(0, 0, 0, 0, 0, 0, p);
}

int
push_unsigned_short(void)
{
        unsigned short p;
        return seventh_unsigned_short(0, 0, 0, 0, 0, 0, p);
}

int
push_int(void)
{
        int p;
        return seventh_int(0, 0, 0, 0, 0, 0, p);
}

int
push_long(void)
{
        long p;
        return seventh_long(0, 0, 0, 0, 0, 0, p);
}

int
push_minus_one_char(void)
{
        return seventh_char(0, 0, 0, 0, 0, 0, -1);
}

int
push_minus_one_signed_char(void)
{
        return seventh_signed_char(0, 0, 0, 0, 0, 0, -1);
}

int
push_minus_one_unsigned_char(void)
{
        return seventh_unsigned_char(0, 0, 0, 0, 0, 0, -1);
}

int
push_minus_one_short(void)
{
        return seventh_short(0, 0, 0, 0, 0, 0, -1);
}

int
push_minus_one_unsigned_short(void)
{
        return seventh_unsigned_short(0, 0, 0, 0, 0, 0, -1);
}

int
push_minus_one_int(void)
{
        return seventh_int(0, 0, 0, 0, 0, 0, -1);
}

int
push_minus_one_long(void)
{
        return seventh_long(0, 0, 0, 0, 0, 0, -1);
}

int
push_wide_char(void)
{
        return seventh_char(0, 0, 0, 0, 0, 0, 0x123456789);
}

int
push_wide_signed_char(void)
{
        return seventh_signed_char(0, 0, 0, 0, 0, 0, 0x123456789);
}

int
push_wide_unsigned_char(void)
{
        return seventh_unsigned_char(0, 0, 0, 0, 0, 0, 0x123456789);
}

int
push_wide_short(void)
{
        return seventh_short(0, 0, 0, 0, 0, 0, 0x123456789);
}

int
push_wide_unsigned_short(void)
{
        return seventh_unsigned_short(0, 0, 0, 0, 0, 0, 0x123456789);
}

int
push_wide_int(void)
{
        return seventh_int(0, 0, 0, 0, 0, 0, 0x123456789);
}

int
push_wide_long(void)
{
        return seventh_long(0, 0, 0, 0, 0, 0, 0x123456789);
}

int
out_char(void)
{
        char p;
        return ninth_char(0, 0, 0, 0, 0, 0, 0, 0, p);
}

int
out_signed_char(void)
{
        signed char p;
        return ninth_signed_char(0, 0, 0, 0, 0, 0, 0, 0, p);
}

int
out_unsigned_char(void)
{
        unsigned char p;
        return ninth_unsigned_char(0, 0, 0, 0, 0, 0, 0, 0, p);
}

int
out_short(void)
{
        short p;
        return ninth_short(0, 0, 0, 0, 0, 0, 0, 0, p);
}

int
out_unsigned_short(void)
{
        unsigned short p;
        return ninth_unsigned_short(0, 0, 0, 0, 0, 0, 0, 0, p);
}

int
out_int(void)
{
        int p;
        return ninth_int(0, 0, 0, 0, 0, 0, 0, 0, p);
}

int
out_long(void)
{
        long p;
        return ninth_long(0, 0, 0, 0, 0, 0, 0, 0, p);
}

int
out_minus_one_char(void)
{
        return ninth_char(0, 0, 0, 0, 0, 0, 0, 0, -1);
}

int
out_minus_one_signed_char(void)
{
        return ninth_signed_char(0, 0, 0, 0, 0, 0, 0, 0, -1);
}

int
out_minus_one_unsigned_char(void)
{
        return ninth_unsigned_char(0, 0, 0, 0, 0, 0, 0, 0, -1);
}

int
out_minus_one_short(void)
{
        return ninth_short(0, 0, 0, 0, 0, 0, 0, 0, -1);
}

int
out_minus_one_unsigned_short(void)
{
        return ninth_unsigned_short(0, 0, 0, 0, 0, 0, 0, 0, -1);
}

int
out_minus_one_int(void)
{
        return ninth_int(0, 0, 0, 0, 0, 0, 0, 0, -1);
}

int
out_minus_one_long(void)
{
        return ninth_long(0, 0, 0, 0, 0, 0, 0, 0, -1);
}
