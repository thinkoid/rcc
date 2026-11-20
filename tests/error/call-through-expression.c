/* A call whose callee is an expression and not a function's name, of a
   type that is not a pointer to a function: 6.5.3.3p1 asks a pointer
   to a function of the expression that denotes the called function. */
int
main(void)
{
        int a = 1;
        int *p = &a;

        return (*p)();
}
