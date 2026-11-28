/* After another type specifier an identifier is the declarator's
   name, so unsigned T declares a T, a redefinition of the typedef
   name. */
typedef int T;
unsigned T x;
