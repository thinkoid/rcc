/* A sum as the pointer of another sum or subscript, on x86-64: by a
   variable index its value, the base the next index joins, addq %rcx,
   %rax at a stride of 1 and leaq (%rax,%rcx,4), %rax at 4; at a stride
   of 1 by an object of 64 bits in memory the addq of the object in
   place, addq -16(%rbp), %rax, under a constant index and a cast too,
   where the access through the sum itself loads the index into the
   form, (%rax,%rcx,1); and by a constant whose bytes 32 bits do not
   hold, the product held and added, movabsq $4000000000, %rcx. */
char g(void);

char
memory_constant(char *p, long l)
{
        return (p + l)[3] + *(p + l + 3) + *(p + l);
}

char
indexes(char *p, int i, int j, long l)
{
        return (p + i)[j] + (i + p)[j] + (p + l)[j] + (l + p)[j];
}

int
casts(char *p, long l, int i)
{
        return *(int *) (p + l) + *(int *) (p + i) + *(short *) (i + p);
}

char *
sums(char *p, int i, int j, long l)
{
        return (p + i) + j + ((p + l) - (i + p));
}

int
wide(int *p, int i, long l)
{
        return (p + 1000000000)[i] + (p + i)[l] + (i + p)[3];
}

void
held(char *p, int i, int j, long l)
{
        (p + i)[j] = g();
        *(p + l) = g();
        (p + l)[i] = g();
        p[l] = g();
}

char *
addresses(char *p, long l, int i)
{
        return &p[l] + (&p[l] - &(p + l)[i]);
}
