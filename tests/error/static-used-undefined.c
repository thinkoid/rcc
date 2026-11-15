/* A static function called and never defined: a name of internal
   linkage that an expression uses has its external definition in the
   unit, 6.9.1p3.  The assembler would leave the symbol undefined, and
   the linker would bind the call to another file's helper. */
static int helper(int x);

int
main(void)
{
        return helper(1);
}
