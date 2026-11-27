/* A case belongs to the innermost switch, 6.8.5.3p4: the inner case 1
   is no duplicate of the outer, and the outer's second is. */
int
f(int a)
{
        int c = 0;
        switch (a) {
        case 1:
                switch (c) {
                case 1: c = 2;
                }
        case 2 - 1: c = 3;
        }
        return c;
}
