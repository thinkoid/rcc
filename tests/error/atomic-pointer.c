/* A pointer qualified _Atomic after its '*', dropped with a warning. */
int
main(void)
{
        int a = 1;
        int *_Atomic p = &a;

        return *p;
}
