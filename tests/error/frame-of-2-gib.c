/* Two locals of 1.5 GB, each under the x86-64 frame limit and their
   sum over it: an error there, which names the function.  On aarch64
   either local alone is too large. */
int
main(void)
{
        char a[1500000000];
        char b[1500000000];

        return a[0] + b[0];
}
