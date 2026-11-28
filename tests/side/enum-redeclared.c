/* A function declared with an enumerated type and with its integer
   type takes the integer type from then on, the composite, 6.2.7p3, as
   clang has it: so a third declaration with another enumeration of that
   integer type agrees. */
enum e { A };
enum f { B };
int g(enum e x);
int g(unsigned x);
int g(enum f x);

int
g(unsigned x)
{
        return x;
}
