/* A subscript of a pointer to an array of unknown size, which is
   incomplete, 6.2.5p27: the pointer of [] points to a complete object
   type, 6.5.3.2p1. */
extern int a[];

int (*p)[] = &a;

int
main(void)
{
        return p[1][0];
}
