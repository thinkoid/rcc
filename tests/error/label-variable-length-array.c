/* A variable length array declared after a label of its function, whose
   declaration a jump back to the label reaches again: rcc does not
   compile it yet. */
int
f(int n)
{
        int k = 0;

again:
        k++;
        {
                char buf[n];

                buf[0] = 1;
                if (k < 2)
                        goto again;
                return buf[0];
        }
}

int
main(void)
{
        return f(1) != 1;
}
