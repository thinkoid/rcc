/* A member access names a member of its record, a record without a tag
   among them, 6.5.3.4p1. */
typedef struct
{
        int a;
} T;

int
f(T *p)
{
        return p->b;
}
