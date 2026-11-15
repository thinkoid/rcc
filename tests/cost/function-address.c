/* backend.md chapter 10 and aarch64.md chapter 10, a function's address
   as a value.  own() returns the address of a function the file
   defines, written defined, and other() that of one the file only
   declares, which may be in a shared library, written &declared: f and
   &f are one value, 6.5.4.2p3, and one listing.

   On x86-64 the listing must show own()'s address formed PC-relative,
   leaq defined(%rip), %rax, as a string literal's is, and other()'s
   loaded from the GOT, movq declared@GOTPCREL(%rip), %rax; each one
   instruction, the result left in rax.

   On aarch64 the listing must show own()'s address formed PC-relative
   in two instructions, adrp x0, defined and add x0, x0, :lo12:defined,
   and other()'s loaded from the GOT in two, adrp x0, :got:declared and
   ldr x0, [x0, :got_lo12:declared]; the result left in x0. */
typedef int (*fn_t)(int);

int
defined(int x)
{
        return x;
}

int declared(int x);

fn_t
own(void)
{
        return defined;
}

fn_t
other(void)
{
        return &declared;
}
