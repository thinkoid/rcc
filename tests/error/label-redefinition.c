/* A label defined twice in a function, 6.8.2p3: an error, though the
   two stand in different blocks, since a label has function scope. */
int
main(void)
{
        {
        again:;
        }
again:
        return 0;
}
