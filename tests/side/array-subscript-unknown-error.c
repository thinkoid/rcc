/* A subscript's pointer points to a complete object type, 6.5.3.2p1:
   no array of unknown size. */
typedef int A[];

int
f(A *p)
{
        return p[0][0];
}
