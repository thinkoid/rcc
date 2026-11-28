/* Pointers to two enumerations point to incompatible types. */
enum e { A = -1 };
enum f { B = -1 };

void
g(enum e *p, enum f *q)
{
        p = q;
}
