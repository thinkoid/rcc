/* Functions and an object whose names are an aarch64 register's: h1,
   x0, w2, sp and Lr, which the assembler reads as a register's in any
   case, and x1.  The operand of a branch is read as a register first,
   so the call of each function names it in quotes (aarch64.md chapter
   8); the object is reached through adrp and :lo12:, which take the
   name.  main() calls each function and reads and writes the object.
   It exits 42 when each holds, or 100 plus the number of the first
   check that failed.

   Under rcc and under clang -O2 it exits 42.

   On x86-64 the listing must show each call as call h1@PLT, the name
   as it is, since a register's name there begins with %.

   On aarch64 the listing must show each call as bl "h1", bl "x0",
   bl "w2", bl "sp" and bl "Lr", and x1 through adrp x16, x1 and
   [x16, :lo12:x1], the name unquoted. */
int x1 = 5;

int
h1(void)
{
        return 1;
}

int
x0(int n)
{
        return n + x1;
}

int
w2(int a, int b)
{
        return a * b;
}

int
sp(void)
{
        return x1 * 2;
}

int
Lr(void)
{
        return 7;
}

int
main(void)
{
        if (h1() != 1)
                return 101;

        if (x0(2) != 7)
                return 102;

        if (w2(3, 4) != 12)
                return 103;

        x1 = 10;
        if (sp() != 20)
                return 104;

        if (Lr() != 7)
                return 105;

        return 42;
}
