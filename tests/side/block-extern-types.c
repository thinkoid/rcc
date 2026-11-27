/* A block's extern of an object of each integer type, the file's
   defined before, defined after and declared only. */
char gc;
long gl = 5;

int
reads(void)
{
        extern char gc;
        extern unsigned short xus;
        extern long gl;
        extern signed char later;
        return gc + xus + gl + later;
}

long
widened(void)
{
        extern int xi;
        return xi;
}

int
stores(int a)
{
        extern unsigned char later2;
        extern long long xll;
        later2 = a;
        xll = 0x123456789;
        return 0;
}

signed char later = -1;
unsigned char later2;
