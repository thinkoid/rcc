/* Chapter 3, f(a, b): the convention's argument moves and call.  The
   listing must show the frame call() sets up because it calls, the
   parameters stored to their slots, b loaded into edi and a into esi,
   the call, and the result left in eax.  The target of the call is a
   relocation, which the listing does not show. */
int f(int a, int b);

int call(int a, int b)
{
    return f(b, a);
}
