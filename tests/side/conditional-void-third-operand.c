/* A void arm and an arm of a value meet in none of 6.5.16p3's pairs:
   an error at the '?'. */
void v(void);

int
main(void)
{
        int a;
        int b;
        return a ? b : v();
}
