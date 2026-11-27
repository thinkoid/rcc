/* Two declarators of one name in a for's init declare it twice in the
   for's scope, 6.7.1p4: the error is said at the second name. */
int
main(void)
{
        for (int i = 0, i = 1; i < 3;)
                ;
        return 0;
}
