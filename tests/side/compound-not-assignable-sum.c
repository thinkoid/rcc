/* a + 1 += 2 is (a + 1) += 2, the additive operator binding first, and
   a sum is no modifiable lvalue. */
void v(void);

int
main(void)
{
        int a = 1;
        int b = 2;
        a + 1 += 2;
        return a;
}
