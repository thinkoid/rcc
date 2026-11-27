/* A constant of 64 bits past int's range is no immediate on x86-64: it
   is movabsq into a register where the source has it, on the right of
   an operator movabsq $4886718345, %rcx and addq %rcx, %rax, and on the
   left of one that commutes too, since only an immediate is exchanged
   to the right; on aarch64 it is the immediate where the instruction
   encodes it, and l & 0x100000000 is a bitmask. */
long
sum(void)
{
        long l = 1;
        return l + 0x123456789;
}

long
sum_on_left(void)
{
        long l = 1;
        return 0x123456789 + l;
}

long
difference_on_left(void)
{
        long l = 1;
        return 0x123456789 - l;
}

long
product(void)
{
        long l = 1;
        return l * 0x123456789;
}

long
mask(void)
{
        long l = 1;
        return l & 0x100000000;
}

long
quotient(void)
{
        long l = 1;
        return l / 0x123456789;
}

long
shifted(void)
{
        long l = 1;
        return l << 0x123456789;
}

int
compared(void)
{
        long l = 1;
        return l < 4294967296;
}

int
compared_on_left(void)
{
        long l = 1;
        return 4294967296 < l;
}

int
condition(void)
{
        long l = 1;
        if (l == 0x123456789)
                return 1;
        return 0;
}
