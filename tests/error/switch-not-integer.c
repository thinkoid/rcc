/* A switch whose controlling expression is a pointer, not an integer,
   6.8.5.3p1: an error. */
int
main(void)
{
        int n = 1;
        int *p = &n;

        switch (p) {
        case 0: return 1;
        }

        return 0;
}
