/* G12: a local keeps its slot after its block. x holds 5 and p points
   to it; y, declared in the next block, has its own slot, so *p after
   both blocks reads 5: exit status 5. clang -O2 marks the end of each
   block's lifetime and stack colouring gives x and y one slot, so *p
   reads the 9 stored in y: 9. printf takes each address so neither
   local is promoted to a register; the two addresses it prints are in
   the log, distinct under rcc and equal under clang. x comes through
   argc. */
int printf(char *, ...);

int main(int argc)
{
    int *p;

    {
        int x = 4 + argc;

        p = &x;
        printf("%p\n", p);
    }
    {
        int y = 9;

        printf("%p\n", &y);
    }

    return *p;
}
