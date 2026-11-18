/* A function declared in a block and called after the block: the
   name's scope ends with the block, 6.2.1p4, and no declaration at
   file scope is visible. */
int
main(void)
{
        {
                int twice(int x);
        }

        return twice(2);
}
