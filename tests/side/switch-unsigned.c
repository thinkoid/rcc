/* A switch on an unsigned, whose cases are sorted unsigned. */
int
f(unsigned x)
{

        switch (x) {
        case 1: return 1;
        case 2: return 2;
        }
        return 0;
}
