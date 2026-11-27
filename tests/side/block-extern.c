/* A block's extern of a name the file does not declare: an object
   declared only, reached through the GOT, and nothing in the data. */
int
f(void)
{
        extern int e;
        return e;
}
