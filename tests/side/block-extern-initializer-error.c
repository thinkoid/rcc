/* A block's extern takes no initializer, 6.7.11p6: the error is at the
   '='. */
int
f(void)
{
        extern int e = 1;
        return e;
}
