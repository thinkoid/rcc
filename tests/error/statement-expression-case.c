/* A case label inside a statement expression whose switch is outside
   it: the switch would jump into the expression, which GNU C does not
   permit, an error as clang has it. */
int
main(void)
{
        int n = 1;

        switch (n) {
        case 0:
                ({
                        case 1: n = 2;
                });
                break;
        }

        return n;
}
