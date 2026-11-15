/* The definition of a function both _Noreturn and inline, which is
   not yet as the definition of an inline function is, whichever
   function specifier stands first: it is an inline definition, which
   provides no external definition. */
_Noreturn inline void
f(void)
{
        for (;;) {
        }
}
