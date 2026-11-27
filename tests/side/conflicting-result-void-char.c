/* Two declarations of one function, of a void result and then of a
   char one, which are not compatible, 6.7.1p5: the error is at the
   later declaration's name. */
void g(void);

char
g(void)
{
        return 0;
}
