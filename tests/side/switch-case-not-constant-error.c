/* A case's expression that does not fold is none. */
int
f(int a)
{
        int b = 0;
        int c = 0;
        switch (a) {
        case (b = 1): c = 1;
        }
        return c;
}
