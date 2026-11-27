/* unsigned is not yet compiled: once the specifiers are read whole, the
   type they name is refused at its first specifier that brings it,
   here 'unsigned' of long unsigned. */
int
f(void)
{
        long unsigned x = 0;
        return 0;
}
