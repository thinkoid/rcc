/* A typedef declared again through a marking attribute of other
   arguments: rcc keeps nothing of what mode makes of int, so it cannot
   tell the two apart, and the second is unsupported, by its words. */
typedef int word_t __attribute__((mode(word)));
typedef int word_t __attribute__((mode(byte)));

int
main(void)
{
        return 0;
}
