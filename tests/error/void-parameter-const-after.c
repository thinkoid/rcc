/* void const as the parameter list, the qualifier after the type: the
   special case is an unnamed parameter of type void, 6.7.7.4p9, and a
   qualified void is a type of its own, 6.2.5p31. */
int f(void const);

int
main(void)
{
        return 0;
}
