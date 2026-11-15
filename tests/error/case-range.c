/* GNU C's case range, case 1 ... 5:, which is not yet. */
int
main(void)
{
        int n = 3;

        switch (n) {
        case 1 ... 5: return 1;
        }

        return 0;
}
