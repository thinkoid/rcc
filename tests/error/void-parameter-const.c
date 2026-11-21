/* A const void as the parameter list: the special case is an unnamed
   parameter of type void, 6.7.7.4p9, and a qualified void is a type of
   its own, 6.2.5p31, whose use there J.2 (75) leaves undefined. */
int f(const void);

int
main(void)
{
        return 0;
}
