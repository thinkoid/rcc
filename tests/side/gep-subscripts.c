/* The subscript through a pointer to each integer type: the index a
   scale of the access, (%rax,%rcx,1), (%rax,%rcx,2), aarch64's shift by
   the size's log and none for a byte, [x0, w1, sxtw], [x0, w1, sxtw
   #1]; a constant the displacement, within aarch64's reach by the
   access's size: p[4095] encodes for every size and p[4096] for none,
   p[-200] on a char * and not on a short *, p[-40] on a short * and not
   on a long *. */
char
char_read(char *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
char_write(char *p, int i, char v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
char_reach(char *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
char_reach_written(char *p, char v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
char_less(char *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
char_cast(char *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

char
char_operand(char *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
char_condition(char *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

char
char_pointer_to_pointer(char **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

signed char
signed_char_read(signed char *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
signed_char_write(signed char *p, int i, signed char v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
signed_char_reach(signed char *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
signed_char_reach_written(signed char *p, signed char v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
signed_char_less(signed char *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
signed_char_cast(signed char *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

signed char
signed_char_operand(signed char *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
signed_char_condition(signed char *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

signed char
signed_char_pointer_to_pointer(signed char **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

unsigned char
unsigned_char_read(unsigned char *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
unsigned_char_write(unsigned char *p, int i, unsigned char v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
unsigned_char_reach(unsigned char *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
unsigned_char_reach_written(unsigned char *p, unsigned char v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
unsigned_char_less(unsigned char *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
unsigned_char_cast(unsigned char *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

unsigned char
unsigned_char_operand(unsigned char *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
unsigned_char_condition(unsigned char *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

unsigned char
unsigned_char_pointer_to_pointer(unsigned char **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

short
short_read(short *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
short_write(short *p, int i, short v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
short_reach(short *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
short_reach_written(short *p, short v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
short_less(short *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
short_cast(short *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

short
short_operand(short *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
short_condition(short *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

short
short_pointer_to_pointer(short **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

unsigned short
unsigned_short_read(unsigned short *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
unsigned_short_write(unsigned short *p, int i, unsigned short v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
unsigned_short_reach(unsigned short *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
unsigned_short_reach_written(unsigned short *p, unsigned short v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
unsigned_short_less(unsigned short *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
unsigned_short_cast(unsigned short *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

unsigned short
unsigned_short_operand(unsigned short *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
unsigned_short_condition(unsigned short *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

unsigned short
unsigned_short_pointer_to_pointer(unsigned short **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

unsigned
unsigned_read(unsigned *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
unsigned_write(unsigned *p, int i, unsigned v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
unsigned_reach(unsigned *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
unsigned_reach_written(unsigned *p, unsigned v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
unsigned_less(unsigned *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
unsigned_cast(unsigned *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

unsigned
unsigned_operand(unsigned *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
unsigned_condition(unsigned *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

unsigned
unsigned_pointer_to_pointer(unsigned **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

long
long_read(long *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
long_write(long *p, int i, long v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
long_reach(long *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
long_reach_written(long *p, long v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
long_less(long *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
long_cast(long *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

long
long_operand(long *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
long_condition(long *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

long
long_pointer_to_pointer(long **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

unsigned long
unsigned_long_read(unsigned long *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
unsigned_long_write(unsigned long *p, int i, unsigned long v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
unsigned_long_reach(unsigned long *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
unsigned_long_reach_written(unsigned long *p, unsigned long v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
unsigned_long_less(unsigned long *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
unsigned_long_cast(unsigned long *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

unsigned long
unsigned_long_operand(unsigned long *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
unsigned_long_condition(unsigned long *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

unsigned long
unsigned_long_pointer_to_pointer(unsigned long **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

long long
long_long_read(long long *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
long_long_write(long long *p, int i, long long v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
long_long_reach(long long *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
long_long_reach_written(long long *p, long long v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
long_long_less(long long *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
long_long_cast(long long *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

long long
long_long_operand(long long *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
long_long_condition(long long *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

long long
long_long_pointer_to_pointer(long long **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}

unsigned long long
unsigned_long_long_read(unsigned long long *p, int i)
{
        return p[i] + i[p] + *(p + i);
}

void
unsigned_long_long_write(unsigned long long *p, int i, unsigned long long v)
{
        p[i] = v;
        i[p] = 7;
        p[0] = 0;
}

long
unsigned_long_long_reach(unsigned long long *p)
{
        return p[3] + p[4095] + p[4096] + p[-1] + p[-40] + p[-200] + p[-256] +
               p[-257] + p[5000];
}

void
unsigned_long_long_reach_written(unsigned long long *p, unsigned long long v)
{
        p[4095] = v;
        p[4096] = v;
        p[-200] = 1;
        p[-40] = v;
}

long
unsigned_long_long_less(unsigned long long *p, int i)
{
        return *(p - i) + *(p - 3) + (p + 1)[i] + (p + i)[2];
}

long
unsigned_long_long_cast(unsigned long long *p, int i)
{
        return *(int *) (p + 2) + *(long *) (p + i) + *(char *) (p + i) +
               ((short *) (p + 1))[i];
}

unsigned long long
unsigned_long_long_operand(unsigned long long *p, long x, int i)
{
        return (x + p[i]) * p[3] / p[i + 1];
}

int
unsigned_long_long_condition(unsigned long long *p, int i)
{
        if (p[i])
                return p[i + 1] == 3;
        return !p[2];
}

unsigned long long
unsigned_long_long_pointer_to_pointer(unsigned long long **pp, int i, int j)
{
        return pp[i][j] + pp[1][2] + *pp[i];
}
