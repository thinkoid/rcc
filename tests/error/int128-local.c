/* GNU C's __int128, as the kernel's headers declare __s128 and __u128:
   the declarations are accepted, and a local, which needs the object's
   size, is not yet.  signed __int128 is __int128. */
typedef __signed__ __int128 s128;
typedef unsigned __int128 u128;

int sum(s128 *a, __int128 *b, u128 *c);

int
main(void)
{
        u128 x;

        return 0;
}
