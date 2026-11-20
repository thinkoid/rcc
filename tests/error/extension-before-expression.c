/* __extension__ before an expression: it is dropped before a
   declaration only, and starts no expression. */
int
main(void)
{
        __extension__ int x = 1;

        __extension__(x);

        return x;
}
