/* G18: a record's assignment copies its object unit by unit, lowest
   address first, with no call. Records of 4 bytes at an alignment of
   2, 8 at 4, 32 at 1 and 72 at 8, the last two over eight units, so
   a loop: each array is rotated by one, through an element by a
   variable index, through pointers, through a global and through a
   local, and two elements are swapped with each place a call's
   result. The checksum of every member, which no padding holds, is
   the exit status, the same under rcc and clang -O2: 4. */
struct code
{
        unsigned char op, bits;
        unsigned short val;
};

struct point
{
        int x, y;
};

struct text
{
        char c[32];
};

struct nine
{
        long a[9];
};

struct code codes[8];
struct point points[8];
struct text texts[4];
struct nine nines[4];
struct nine spare;

struct code *
at(struct code *t, int i)
{
        return &t[i];
}

void
fill(void)
{
        int i, k;

        for (i = 0; i < 8; i++) {
                codes[i].op = (unsigned char) (i * 7 + 1);
                codes[i].bits = (unsigned char) (i + 3);
                codes[i].val = (unsigned short) (i * 1000 + 17);
                points[i].x = i * 11 - 5;
                points[i].y = i * i;
        }

        for (i = 0; i < 4; i++)
                for (k = 0; k < 32; k++)
                        texts[i].c[k] = (char) (i * 32 + k);

        for (i = 0; i < 4; i++)
                for (k = 0; k < 9; k++)
                        nines[i].a[k] = (long) i * 1000003 + k;
}

void
rotate(void)
{
        struct code first;
        struct text last;
        struct point *p;
        int i;

        first = codes[0];
        for (i = 0; i < 7; i++)
                codes[i] = codes[i + 1];
        codes[7] = first;

        for (p = points; p < points + 7; p++)
                *p = *(p + 1);

        spare = nines[0];
        for (i = 0; i < 3; i++)
                nines[i] = nines[i + 1];
        nines[3] = spare;

        last = texts[3];
        for (i = 3; i > 0; i--)
                texts[i] = texts[i - 1];
        texts[0] = last;

        first = *at(codes, 2);
        *at(codes, 2) = *at(codes, 5);
        *at(codes, 5) = first;
}

unsigned long
sum(void)
{
        unsigned long s = 0;
        int i, k;

        for (i = 0; i < 8; i++)
                s = s * 31 + codes[i].op + codes[i].bits * 3 +
                    codes[i].val * 7 + (unsigned long) points[i].x * 5 +
                    (unsigned long) points[i].y;

        for (i = 0; i < 4; i++) {
                for (k = 0; k < 32; k++)
                        s = s * 31 + (unsigned char) texts[i].c[k];
                for (k = 0; k < 9; k++)
                        s = s * 31 + (unsigned long) nines[i].a[k];
        }

        return s;
}

int
main(void)
{
        fill();
        rotate();
        rotate();

        return (int) (sum() % 251);
}
