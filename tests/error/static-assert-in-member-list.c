/* A static assertion in a struct's member list, 6.7.3.2p1, checked
   where it stands, 6.7.12p2. */
struct pair
{
        int a, b;
        _Static_assert(sizeof(int) == 2, "int is 16 bits");
};

int
main(void)
{
        return 0;
}
