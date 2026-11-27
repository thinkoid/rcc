/* Nor is it the statement after a case label where a statement is
   wanted. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a)
        case 1: int d; return 0;
}
