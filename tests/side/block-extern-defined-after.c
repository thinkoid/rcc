/* A block's extern of an object the file defines after it: the file's
   definition adopts the block's entity, and the read is the label's. */
int
f(void)
{
        extern int g;
        return g;
}
int g = 1;
