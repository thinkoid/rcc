/* A static assertion in a member list is not yet, where it stands. */
struct s
{
        int a;
        _Static_assert(1, "one");
};
