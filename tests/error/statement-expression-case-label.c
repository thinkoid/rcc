/* A statement expression as a case label's constant, which clang folds
   to its value, GNU C's extension: rcc does not fold one yet. */
int
main(void)
{
        int n = 1;

        switch (n) {
        case ({ 1; }): return 1;
        }

        return 0;
}
