/* Two void arms make a void conditional, 6.5.16p6, whose value a
   return reads: the error is the return's. */
void v(void);
void w(void);

int
main(void)
{
        int a;
        return a ? v() : w();
}
