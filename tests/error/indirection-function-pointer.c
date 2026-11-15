/* *p on a pointer to a function: the function designator it yields is
   not yet. */
int
call(int (*p)(void))
{
        return (*p)();
}

int
main(void)
{
        return 0;
}
