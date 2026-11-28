/* An enumerated type is its compatible integer type to every row,
   6.7.3.3p13, clang's choice: unsigned int where no value is negative,
   int where one is, and the 64-bit types where a value needs them.  An
   object, a parameter, a result and a cast of one are of that type: x -
   1 < 0 compares unsigned, setb and lo, or signed, setl and lt, and a
   type of 64 bits divides at 64 bits.  A pointer to an enumeration
   compatible with int points to it. */
enum u { ua, ub, uc = 7 };
enum i { ia = -1, ib, ic = 7 };
enum ul { ula, ulb = 0x100000000 };
enum l { la = -1, lb = 0x100000000 };

enum u gu = uc;
enum l gl = lb;

int
unsigned_compare(enum u x)
{
        return x - 1 < 0;
}

int
signed_compare(enum i x)
{
        return x - 1 < 0;
}

long
wide_divide(enum ul x)
{
        return x / 2;
}

enum l
wide_signed(enum l x)
{
        enum l y = x;
        return y >> 1;
}

enum u
converted(long x)
{
        return (enum u) x;
}

int
pointed(enum i *p)
{
        int x = 1;
        int *q = &x;
        p = q;
        return p == q;
}

unsigned long
sizes(void)
{
        return sizeof(enum u) + sizeof(enum i) * 10 + sizeof(enum ul) * 100 +
               sizeof(enum l) * 1000;
}
