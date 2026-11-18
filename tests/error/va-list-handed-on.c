/* A list handed on to the C library: on x86-64 the list is the address
   of its record, which passes as any pointer, so the program compiles;
   on aarch64 the record, which passes as the address of a copy the
   caller makes, and c1 does not make the copy yet. */
#include <stdarg.h>
#include <stdio.h>

int
report(const char *form, va_list ap)
{
        return vprintf(form, ap);
}

int
main(void)
{
        return 0;
}
