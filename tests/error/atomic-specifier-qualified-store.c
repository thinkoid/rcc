/* A store into an object of the atomic type specifier on a qualified
   type name: the type name's const is joined with the qualifiers beside
   the specifier, as a typedef name's is, 6.7.4.1p6, so y is a const
   object, which 6.5.17.1p2 does not take as a left operand. */
_Atomic(const int) y;

int
main(void)
{
        y = 1;
        return 0;
}
