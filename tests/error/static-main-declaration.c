/* main declared static before its definition, which takes the
   declaration's internal linkage, 6.2.2p3 and p5: the link would find
   no main. */
static int main(void);

int
main(void)
{
        return 0;
}
