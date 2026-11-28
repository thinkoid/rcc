/* A typedef name is an ordinary identifier, 6.2.3p1, with the scope of
   its declaration, 6.2.1p4.  A typedef in a block ends with it and hides
   an object of the file's; an object of a block hides a typedef name to
   the block's end, and so does a parameter in the body; T T = 1;
   declares an object T of the outer typedef's type, the object's scope
   beginning after its declarator, 6.2.1p7; and a label T is of a name
   space of its own. */
typedef int T;
int g;

long
inner(void)
{
        typedef long T;
        T y = 1;
        return sizeof y + y;
}

unsigned long
inner_ends(void)
{
        {
                typedef long T;
        }
        return sizeof(T);
}

int
hides_object(void)
{
        typedef char g;
        g c = 1;
        return c;
}

int
object_hides(void)
{
        {
                int T = 1;
                g = T;
        }
        T y = 2;
        return y;
}

int
parameter_hides(int T)
{
        return T + 1;
}

int
same_name(void)
{
        T T = 1;
        return T;
}

int
label(void)
{
        goto T;
T:
        return 0;
}

int
in_for(void)
{
        int n = 0;
        for (T i = 0; i < 3; i++)
                n += i;
        return n;
}
