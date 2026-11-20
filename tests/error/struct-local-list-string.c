/* A string literal for a member array of char in a local record's
   list, 6.7.11p15: an object of static storage holds the literal's
   bytes as the array's own, and a local's array would take them at run
   time, which is not yet. */
struct name
{
        char text[4];
        int count;
};

int
main(void)
{
        struct name one = { "abc", 1 };

        return one.count;
}
