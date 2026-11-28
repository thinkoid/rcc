/* A typedef name of an array names the array type: an object of it is
   an array, a parameter of it a pointer to its element, and a pointer
   to it points to the array, whose * is the array again. */
typedef int A[4];
typedef A *P;

int
objects(int i)
{
        A a;
        A *p = &a;
        P q = p;
        a[i] = 1;
        (*p)[1] = 2;
        return (*q)[i] + **p + sizeof(A) + sizeof *p + sizeof(P);
}

int
parameter(A a, int i)
{
        return a[i];
}
