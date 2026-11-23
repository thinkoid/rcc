/* A static inline function never called, whose two locals of 10 MB,
   each under the aarch64 frame limit, pass it together: the frame is
   an error of the program whether the function is called or not, so
   the function is not dropped, and the error names it.  x86-64
   compiles it, and writes the function. */
static inline int
f(void)
{
        char a[10000000];
        char b[10000000];

        return a[0] + b[0];
}

int
main(void)
{
        return 0;
}
