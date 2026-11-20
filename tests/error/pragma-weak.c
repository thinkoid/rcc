/* #pragma weak makes a symbol weak: clang writes .weak for the
   declared helper, whose address is then null where no unit defines
   it, and rcc writes its reference strong, so the program fails at
   link.  rcc refuses the pragma by its name, as not yet. */
#pragma weak helper

int helper(void);

int
main(void)
{
        return helper ? helper() : 0;
}
