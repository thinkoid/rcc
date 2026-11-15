/* An object defined by an extern declaration with an initializer,
   6.9.3p1, and again by one without extern: two external definitions,
   6.9.1p5. */
extern int count = 1;
int count = 2;

int
main(void)
{
        return count;
}
