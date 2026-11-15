/* p + 1 on a pointer to a function: pointer arithmetic needs a pointer
   to a complete object type, 6.5.7p2. */
int (*next(int (*p)(void)))(void)
{
        return p + 1;
}

int
main(void)
{
        return 0;
}
