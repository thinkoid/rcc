/* Parameters past the registers.  On x86-64 the seventh and later stay
   where the caller pushed them, 8 bytes apart whatever their types,
   and are read and written there at their types, movsbl 16(%rbp),
   %eax and movb $5, 32(%rbp).  On aarch64 the ninth and later are
   copied to their slots at their own width through w17 or x17 by the
   plain load, ldrb w17, [x29, #64] and strb w17, [x29, #48], read or
   not, and read from the slots. */
char
seventh_char(int a1, int a2, int a3, int a4, int a5, int a6, char x)
{
        return x;
}

signed char
seventh_signed_char(
    int a1, int a2, int a3, int a4, int a5, int a6, signed char x)
{
        return x;
}

unsigned char
seventh_unsigned_char(
    int a1, int a2, int a3, int a4, int a5, int a6, unsigned char x)
{
        return x;
}

short
seventh_short(int a1, int a2, int a3, int a4, int a5, int a6, short x)
{
        return x;
}

unsigned short
seventh_unsigned_short(
    int a1, int a2, int a3, int a4, int a5, int a6, unsigned short x)
{
        return x;
}

int
seventh_int(int a1, int a2, int a3, int a4, int a5, int a6, int x)
{
        return x;
}

long
seventh_long(int a1, int a2, int a3, int a4, int a5, int a6, long x)
{
        return x;
}

char
ninth_char(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, char x)
{
        return x;
}

signed char
ninth_signed_char(int a1,
                  int a2,
                  int a3,
                  int a4,
                  int a5,
                  int a6,
                  int a7,
                  int a8,
                  signed char x)
{
        return x;
}

unsigned char
ninth_unsigned_char(int a1,
                    int a2,
                    int a3,
                    int a4,
                    int a5,
                    int a6,
                    int a7,
                    int a8,
                    unsigned char x)
{
        return x;
}

short
ninth_short(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, short x)
{
        return x;
}

unsigned short
ninth_unsigned_short(int a1,
                     int a2,
                     int a3,
                     int a4,
                     int a5,
                     int a6,
                     int a7,
                     int a8,
                     unsigned short x)
{
        return x;
}

int
ninth_int(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int x)
{
        return x;
}

long
ninth_long(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, long x)
{
        return x;
}

int
ninth_stored_char(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, char x)
{
        x = 5;
        return 0;
}

int
ninth_stored_short(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, short x)
{
        x = 5;
        return 0;
}

int
ninth_stored_int(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int x)
{
        x = 5;
        return 0;
}

int
ninth_stored_long(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, long x)
{
        x = 5;
        return 0;
}

int
ninth_unread_char(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, char x)
{
        return 0;
}

int
ninth_unread_short(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, short x)
{
        return 0;
}

int
ninth_unread_int(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int x)
{
        return 0;
}

int
ninth_unread_long(
    int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, long x)
{
        return 0;
}

int
stacked_widened0(char a,
                 long b,
                 short c,
                 int d,
                 unsigned char e,
                 unsigned long f,
                 short g,
                 char h,
                 long i)
{
        long r;
        r = g;
        r = h;
        r = i;
        return 0;
}

int
stacked_widened1(int a1,
                 int a2,
                 int a3,
                 int a4,
                 int a5,
                 int a6,
                 char g,
                 long h,
                 short i,
                 unsigned char j,
                 long k)
{
        long r;
        r = g;
        r = h;
        r = i;
        r = j;
        r = k;
        return 0;
}

int
stacked_widened2(int a1,
                 int a2,
                 int a3,
                 int a4,
                 int a5,
                 int a6,
                 int a7,
                 int a8,
                 char i,
                 short j,
                 long k,
                 unsigned char l)
{
        long r;
        r = i;
        r = j;
        r = k;
        r = l;
        return 0;
}

int
stacked_widened3(int a1,
                 int a2,
                 int a3,
                 int a4,
                 int a5,
                 int a6,
                 int a7,
                 int a8,
                 long i,
                 char j,
                 short k,
                 int l)
{
        long r;
        r = i;
        r = j;
        r = k;
        r = l;
        return 0;
}
