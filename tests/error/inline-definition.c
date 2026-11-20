/* The definition of an inline function, which is not yet: it is an
   inline definition, which provides no external definition.  On the
   declaration before it, inline is dropped. */
inline int twice(int x);

inline int
twice(int x)
{
        return x + x;
}
