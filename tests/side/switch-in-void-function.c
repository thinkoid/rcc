/* A switch in a void function, its cases returning without a value. */
void
f(int a)
{
        int b = 0;
        switch (a) {
        case 1: b = 1; return;
        case 2: b = 2; break;
        default: return;
        }
        b = 3;
}
