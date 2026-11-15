/* A pointer to a function compared with a void * that is not a null
   pointer constant: 6.5.10p2 lets a pointer to void meet a pointer to
   an object type only.  The null pointer constant (void *) 0 meets
   it. */
int
same(void (*h)(int), void *v)
{
        return h != (void *) 0 && h == v;
}

int
main(void)
{
        return 0;
}
