/* A block's extern of a name the file does not declare, unused: it
   makes no object and writes nothing. */
int
f(void)
{
        extern int e;
        return 0;
}
