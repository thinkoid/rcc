/* A typedef name, 6.7.9p3, stands for its type wherever a type is read,
   and writes nothing of its own: each function's listing is the one of
   the type spelled out.  A declaration's specifiers, a parameter, a
   result, a cast, sizeof and the pointee of a pointer read it; a
   typedef of a pointer names the pointer, and one of void is a void
   result and an empty parameter list.  A typedef name declared again
   names the same type, 6.7.1p4, signed int being int. */
typedef int T;
typedef signed int T;
typedef unsigned char U;
typedef long L;
typedef T *P;
typedef void V;

U g = 200;

L
widened(T x, U u)
{
        L l = x;
        return l + u;
}

T
cast(L l)
{
        return (U) l + (T) l;
}

unsigned long
sizes(void)
{
        return sizeof(T) + sizeof(U) + sizeof(L) + sizeof(P) + sizeof(T *);
}

P
pointed(P p)
{
        T *q = p;
        return q;
}

int
void_list(V)
{
        static U s = 3;
        extern U g;
        return s + g;
}

V
nothing(void)
{
}
