/* A string literal longer than the array of char a member is, which
   would initialize bytes the member does not contain, 6.7.11p3: an
   error. */
struct name
{
        char text[3];
        int count;
};

struct name one = { "four", 1 };

int
main(void)
{
        return one.count;
}
