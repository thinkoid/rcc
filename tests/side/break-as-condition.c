/* A break where a do's condition stands: break is no expression. */
int
main(void)
{
        do
                ;
        while (break);
        return 0;
}
