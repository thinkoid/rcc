/* The frame of narrow locals among wider ones, A2 of the survey: each
   takes its own bytes at its own alignment, in declaration order, char
   c; int i; char d; long l; short s; at -1, -8, -9, -24, -26 on x86-64
   and #16, #20, #24, #32, #40 on aarch64; the stores are movb, movw,
   strb and strh. */
int
mixed(void)
{
        char c;
        int i;
        char d;
        long l;
        short s;
        c = 1;
        i = 2;
        d = 3;
        l = 4;
        s = 5;
        return 0;
}

int
many(void)
{
        long l;
        char c;
        short s;
        char d;
        unsigned long long q;
        unsigned char e;
        int i;
        l = 1;
        c = 2;
        s = 3;
        d = 4;
        q = 5;
        e = 6;
        i = 7;
        return 0;
}

int
spill_after_char(void)
{
        char c = 1;
        int a = 2;
        return a + (a + (a + (a + (a + (a + (a + (a + (a + (a + c)))))))));
}
