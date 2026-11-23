/* A typedef through aligned written again with another alignment: rcc
   keeps nothing of what aligned makes of int, so it cannot tell
   whether the two are the same type, and the second typedef is not
   yet, by the attribute's words. */
typedef int wide_t __attribute__((aligned(8)));
typedef int wide_t __attribute__((aligned(16)));

int
main(void)
{
        return 0;
}
