/* Lists of mixed types, of four, six and more: the slots in parameter
   order by the locals' rule, each stored from its argument register at
   its width, movw %r8w, -28(%rbp); past the registers on aarch64 each
   copied at its width through w17 or x17 by the plain load, ldrh w17,
   [x29, #80] and strh w17, [x29, #56]; and the locals' slots after the
   parameters', aligned by their own types. */
int
list0(char a, long b, short c, int d)
{
        return 0;
}

int
list1(char a, long b, short c, int d, unsigned char e, unsigned long f)
{
        return 0;
}

int
list2(long a, char b, long c, char d, short e, int g)
{
        return 0;
}

int
list3(char a, char b, char c, char d, char e, char g)
{
        return 0;
}

int
list4(short a, int b, char c, long d, unsigned short e, signed char g)
{
        return 0;
}

int
list5(char a,
      long b,
      short c,
      int d,
      unsigned char e,
      unsigned long f,
      short g,
      char h,
      long i)
{
        return 0;
}

int
list6(int a1,
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
        return 0;
}

int
list7(int a1,
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
        return 0;
}

int
list8(int a1,
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
        return 0;
}

int
list0_locals(char a, long b, short c, int d)
{
        char x;
        long y;
        x = 1;
        y = 2;
        return 0;
}

int
list1_locals(char a, long b, short c, int d, unsigned char e, unsigned long f)
{
        char x;
        long y;
        x = 1;
        y = 2;
        return 0;
}

int
list2_locals(long a, char b, long c, char d, short e, int g)
{
        char x;
        long y;
        x = 1;
        y = 2;
        return 0;
}

int
list3_locals(char a, char b, char c, char d, char e, char g)
{
        char x;
        long y;
        x = 1;
        y = 2;
        return 0;
}

int
list4_locals(short a, int b, char c, long d, unsigned short e, signed char g)
{
        char x;
        long y;
        x = 1;
        y = 2;
        return 0;
}
