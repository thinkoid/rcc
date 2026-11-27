/* A switch on a long, whose dispatch is at 64 bits: not yet. */
int
f(long x)
{

        switch (x) {
        case 1: return 1;
        case 2: return 2;
        }
        return 0;
}
