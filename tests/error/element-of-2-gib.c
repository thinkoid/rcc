/* Arithmetic on a pointer to an element of 2 GiB, whose stride is not
   the 32-bit immediate of x86-64: not yet there.  aarch64 compiles
   it. */
int
f(char a[][2147483648])
{
        return a[1][0];
}
