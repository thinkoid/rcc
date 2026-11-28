/* An enumeration constant is an ordinary identifier and its tag a tag,
   each of the scope of its specifier, 6.2.1p4: a constant of a block
   hides an object of the file's, and an object of a block a constant; a
   constant's scope begins after its enumerator, 6.2.1p7, so an inner
   list's A = A + 1 reads the outer A; a block's list defines a tag the
   file defines already; a tag named again is the enumerated type; an
   enum in a typedef and in a parameter's type, whose constants the body
   sees. */
enum e { A = 1 };
int B = 5;

int
hides_object(void)
{
        enum { B = 1 };
        return B;
}

int
object_hides(void)
{
        int A = 2;
        return A;
}

int
inner(void)
{
        enum { A = A + 1 };
        return A;
}

int
retagged(void)
{
        enum e { C };
        enum e x = C;
        return x;
}

enum e;
enum e named = A;

typedef enum { D, E } T;

T
typed(T x)
{
        return x + E;
}

int parameter(enum p { F, G } x)
{
        return x == G;
}
