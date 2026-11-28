/* A case's expression of a pointer type is no integer constant. */
int
f(int a)
{
        switch (a) {
        case (int *) 0: return 1;
        }
        return 0;
}
