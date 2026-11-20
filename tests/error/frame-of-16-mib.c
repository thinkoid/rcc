/* Two locals of 10 MB, each under the aarch64 frame limit and their
   sum over it: a frame of 16 MiB or more is not yet there.  x86-64
   compiles it. */
int
main(void)
{
        char a[10000000];
        char b[10000000];

        return a[0] + b[0];
}
