/* A statement expression as a case label's constant, which clang folds
   to its value: it is no operand of a constant expression, 6.6p8. */
int
main(void)
{
        int n = 1;

        switch (n) {
        case ({ 1; }): return 1;
        }

        return 0;
}
