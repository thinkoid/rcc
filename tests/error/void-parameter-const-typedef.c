/* A typedef name of a const void as the parameter list: the typedef
   names the qualified void, 6.7.4.1p6, and the special case is an
   unnamed parameter of type void, 6.7.7.4p9. */
typedef const void nothing;

int f(nothing);

int
main(void)
{
        return 0;
}
