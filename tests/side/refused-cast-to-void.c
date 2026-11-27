/* A cast to void, which discards its operand's value, 6.5.5p2, is not
   yet compiled, refused at the '('. */
int
f(void)
{
        int a = 1;
        (void) a;
        return a;
}
