/* A subscript of a pointer to void: the pointer of [] points to a
   complete object type, 6.5.3.2p1, and void is incomplete. */
int
f(void *v)
{
        return v[0];
}

int
main(void)
{
        return 0;
}
