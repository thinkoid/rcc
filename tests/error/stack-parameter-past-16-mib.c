/* A frame just under the aarch64 limit puts the third parameter past
   the eighth 16 MiB above the frame record, out of reach of the add
   into x16: not yet there.  x86-64 compiles it. */
long f(long a, long b, long c, long d, long e, long g, long h, long i,
       long j, long k, long l)
{
    char big[16777088];

    return l + big[0];
}
