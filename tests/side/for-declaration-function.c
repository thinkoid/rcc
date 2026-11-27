/* A for's declaration declares objects only, by C17's 6.8.5p3, so a
   function declared there is rcc's error, said at its name. */
int
main(void)
{
        for (int g(void);;)
                ;
        return 0;
}
