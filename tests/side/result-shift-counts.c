/* A call's result of each type as a shift's count, which keeps its
   own promoted type.  On x86-64 the count's extension is written
   straight into ecx, movsbl %al, %ecx, so the shifted value is
   reloaded into rax; an int count stays in eax, the shifted value
   is reloaded into rdx and the count moved, movl %eax, %ecx. */
char result_char(void);
signed char result_signed_char(void);
unsigned char result_unsigned_char(void);
short result_short(void);
unsigned short result_unsigned_short(void);
int result_int(void);
unsigned result_unsigned(void);
long result_long(void);
unsigned long result_unsigned_long(void);
long long result_long_long(void);
unsigned long long result_unsigned_long_long(void);

int
shift_counts_char(void)
{
        int a;
        return a << result_char();
}

int
shift_counts_signed_char(void)
{
        int a;
        return a << result_signed_char();
}

int
shift_counts_unsigned_char(void)
{
        int a;
        return a << result_unsigned_char();
}

int
shift_counts_short(void)
{
        int a;
        return a << result_short();
}

int
shift_counts_unsigned_short(void)
{
        int a;
        return a << result_unsigned_short();
}

int
shift_counts_int(void)
{
        int a;
        return a << result_int();
}

int
shift_counts_unsigned(void)
{
        int a;
        return a << result_unsigned();
}

int
shift_counts_long(void)
{
        int a;
        return a << result_long();
}

int
shift_counts_unsigned_long(void)
{
        int a;
        return a << result_unsigned_long();
}

int
shift_counts_long_long(void)
{
        int a;
        return a << result_long_long();
}

int
shift_counts_unsigned_long_long(void)
{
        int a;
        return a << result_unsigned_long_long();
}
