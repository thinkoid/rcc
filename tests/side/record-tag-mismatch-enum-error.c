/* The tags of structs, unions and enums are one name space, 6.2.3p1, so
   an enum of a struct's tag is the error of 6.7.3.4p2. */
struct s
{
        int a;
};
int f(enum s x);
