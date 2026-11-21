/* A pointer to unsigned char assigned to a pointer to char: char is a
   type of its own, 6.2.5p20, whichever of signed char and unsigned
   char it behaves as, so 6.5.17.2p1 asks a cast on aarch64 too, where
   char is unsigned. */
int
main(void)
{
        unsigned char c = 1;
        unsigned char *p = &c;
        char *q = p;

        return 0;
}
