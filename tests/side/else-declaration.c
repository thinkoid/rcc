/* A declaration as an if's else part, which is a statement too. */
int
main(void)
{
        int a = 1;
        if (a)
                ;
        else
                int b;
        return 0;
}
