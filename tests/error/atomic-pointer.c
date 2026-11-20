/* A pointer qualified _Atomic after its '*', which is not yet. */
int
main(void)
{
        int a = 1;
        int *_Atomic p = &a;

        return *p;
}
