/* A do's while without the '(' of its condition, said at the token
   found in its place. */
int
main(void)
{
        int a = 1;
        do
                ;
        while a;
        return 0;
}
