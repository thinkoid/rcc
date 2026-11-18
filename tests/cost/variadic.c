/* shape.md chapter 5: variadic definitions, 7.16.  va_start builds the
   list from the function's save area, which the prologue fills with
   every argument register, general and vector, and the address of the
   arguments on the stack; va_arg reads the next 8-byte slot, the save
   area's while a register is left and then the stack's, at the type's
   width; va_copy is a record's assignment; va_end is the list's address,
   discarded; and a list is handed to the C library, on x86-64 as the
   address of its record and on aarch64 as the address of a copy the
   caller makes. */
#include <stdarg.h>
#include <stdio.h>

long
sum(int count, ...)
{
        va_list ap;
        long s = 0;

        va_start(ap, count);
        while (count-- > 0)
                s += va_arg(ap, long);
        va_end(ap);

        return s;
}

int
twice(int count, ...)
{
        va_list ap, again;
        int s = 0;

        va_start(ap, count);
        va_copy(again, ap);
        for (int n = count; n > 0; --n)
                s += va_arg(ap, int) + va_arg(again, int);
        va_end(again);
        va_end(ap);

        return s;
}

int
print(const char *form, ...)
{
        va_list ap;
        int n;

        va_start(ap, form);
        n = vprintf(form, ap);
        va_end(ap);

        return n;
}
