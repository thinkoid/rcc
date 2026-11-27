/* A case label after its switch is in none. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case 1: b = 1;
        }
case 2: return b;
}
