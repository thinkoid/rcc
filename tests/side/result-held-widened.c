/* The width rule across a call: an unsigned sum is valid to 64 bits
   by its 32-bit write, so its widening writes nothing after the addl,
   where a call's unsigned result widened is movl %eax, %eax; an int
   sum is movslq. */
char char_result(void);
int int_result(void);
unsigned unsigned_result(void);

long
widened_char_sum(void)
{
        char x;
        return x + char_result();
}

long
widened_int_sum(void)
{
        int x;
        return x + int_result();
}

long
widened_unsigned_sum(void)
{
        unsigned x;
        return x + unsigned_result();
}

long
widened_unsigned_difference(void)
{
        unsigned x;
        return x - unsigned_result();
}
