/* Expression statements, whose value is dropped, 6.8.4p2, and null
   statements, which perform no operations, p3.  A name alone is still
   its load and a constant alone still its mov, in a register taken for
   it and released, since the statement evaluates its expression.  An
   empty block writes nothing either.

   name() holds a local alone and constants() 7 and 0 alone; null()
   holds null statements, among them one in a block, and an empty
   block.  dead() and main() write the statements after their return,
   which no path reaches but which are generated, a declaration's slot
   among them; main() writes no return of 0 after them, since the
   return before them leaves no path to its end. */
int
name(void)
{
        int x = 1;
        x;
        return 2;
}

int
constants(void)
{
        7;
        0;
        return 3;
}

int
null(void)
{
        ;
        {
                ;
        }
        {
        }
        return 4;
}

int
dead(void)
{
        int x = 1;
        return x;
        x = 2;
        int y = x;
}

int
main(void)
{
        int x = 1;
        return x;
        int y = 2;
        y;
}
