/* A do's condition without its closing ')', said at the token found
   in its place. */
int
main(void)
{
        int a = 1;
        do
                ; while (a;
        return 0;
}
