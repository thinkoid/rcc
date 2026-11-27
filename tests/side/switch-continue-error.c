/* A continue stands only in a loop, 6.8.7.3p1, and a switch is none. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: continue;
        }
        return 0;
}
