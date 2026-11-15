/* An array declared twice with two sizes: two array types are
   compatible only when their sizes are equal or one is left out,
   6.7.7.3p6, so the types conflict. */
extern int table[2];
extern int table[3];

int
main(void)
{
        return 0;
}
