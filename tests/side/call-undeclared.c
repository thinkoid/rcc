/* A call of a name no declaration in scope makes: the error is at the
   name, 6.5.2p3. */
int
f(void)
{
        return g();
}
