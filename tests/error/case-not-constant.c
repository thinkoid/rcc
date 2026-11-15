/* A case label whose expression is no integer constant expression,
   6.8.5.3p3: an error.  An object's value is no constant, 6.6p8. */
int
main(void)
{
        int n = 1;
        int k = 1;

        switch (n) {
        case k: return 1;
        }

        return 0;
}
