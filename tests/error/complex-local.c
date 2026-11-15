/* Complex types, as complex.h declares its functions with them: the
   declarations are accepted, and a local, which needs the object's
   size and layout, is not yet. */
extern double _Complex cacos(double _Complex z);

long double _Complex cl(float _Complex, _Complex long double);

int
main(void)
{
        double _Complex z;

        return 0;
}
