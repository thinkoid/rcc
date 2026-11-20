/* A typedef whose type mode changes, as sys/types.h declares
   register_t: the typedef is accepted and names a type of its own,
   and a local of that type, which needs its size, is not yet.  A
   pointer to it is an ordinary pointer. */
typedef int word_t __attribute__((__mode__(__word__)));

word_t *first(word_t *p);

int
main(void)
{
        word_t w;

        return 0;
}
