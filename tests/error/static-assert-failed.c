/* A static assertion whose expression compares equal to 0: the
   constraint of 6.7.12p2 is violated, and the diagnostic includes the
   string, p3, its adjacent literals joined. */
_Static_assert(sizeof(int) == 8,
               "int is "
               "64 bits");

int
main(void)
{
        return 0;
}
