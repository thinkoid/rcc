/* *p through a pointer to each integer type as a condition: x86-64
   compares a narrow object in place at its width, cmpb $0, (%rax),
   cmpw $0, (%rax), but !*p loads it; aarch64 loads every one, cbz w0. */
enum e { A = -1, B = 7 };

int
char_conditions(char *p, int x, char **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
signed_char_conditions(signed char *p, int x, signed char **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
unsigned_char_conditions(unsigned char *p, int x, unsigned char **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
short_conditions(short *p, int x, short **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
unsigned_short_conditions(unsigned short *p, int x, unsigned short **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
unsigned_conditions(unsigned *p, int x, unsigned **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
long_conditions(long *p, int x, long **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
unsigned_long_conditions(unsigned long *p, int x, unsigned long **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
long_long_conditions(long long *p, int x, long long **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
unsigned_long_long_conditions(unsigned long long *p,
                              int x,
                              unsigned long long **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}

int
enum_conditions(enum e *p, int x, enum e **pp)
{
        if (*p)
                x = x + 1;
        if (!*p)
                x = x + 2;
        if (*p == 5)
                x = x + 3;
        if (*p != 0)
                x = x + 4;
        if (x < *p)
                x = x + 5;
        if (*p & 1)
                x = x + 6;
        if (**pp)
                x = x + 7;
        while (*p)
                *p = 0;
        return x ? *p : !*p;
}
