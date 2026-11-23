/* A local of 16 MiB, larger than the aarch64 frame limit: an error at
   its name there.  x86-64 compiles it. */
int
main(void)
{
        char a[16777216];

        return a[0];
}
