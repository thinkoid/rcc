/* main falling off its end after calls returns 0, movl $0, %eax and mov
   w0, #0 after the last call; a main that returns a call's value
   writes only the jump. */
int putchar(int c);
void helper(void);
int twice(int a);

int
main(void)
{
        putchar(72);
        putchar(10);
        helper();
}

int
main2(void)
{
        return twice(21);
}
