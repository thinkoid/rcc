/* A generic selection writes its result expression alone: its
   controlling expression and the expressions of the associations it
   does not select are not evaluated, 6.5.2.1p3, and the tree keeps the
   node of the one it selects, of that expression's type and value and
   an lvalue where it is one, p4.  Each function selects by a type the
   machine's view does not tell apart from another: long from long
   long, char from signed char or unsigned char, an enumeration from
   another, a pointer to const from a pointer to its unqualified type,
   and int from an association of const int, which no controlling
   expression selects, p2's footnote.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with each selection replaced by the
   expression it selects: x + 1 in by_kind(), y * 2 in by_default(),
   *p in assigned(), 2 in by_char(), c + 2 in by_enumeration(), n in
   unevaluated(), with no step of n, twice in by_designator(), and in
   the shape of glibc's __glibc_const_generic 1, the const void *
   association's, in by_pointee() and 2, the default's, in
   by_plain_pointee().

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
enum color { red, green };
enum shape { round, square };

int
by_kind(int x)
{
        return _Generic(1L, long: x + 1, long long: x + 2, default: 0);
}

long
by_default(long y)
{
        return _Generic(y, int: 0, long long: 1, default: y * 2);
}

void
assigned(int *p)
{
        _Generic(*p, const int: p[1], int: *p) = 3;
}

int
by_char(char c)
{
        return _Generic(c, signed char: 1, char: 2, unsigned char: 3);
}

int
by_enumeration(enum color c)
{
        return _Generic(c, enum shape: 1, enum color: c + 2);
}

int
unevaluated(int n)
{
        return _Generic(n++, int: n);
}

int
twice(int x)
{
        return 2 * x;
}

int
by_designator(int x)
{
        return _Generic(x, int: twice, default: 0)(x);
}

int
by_pointee(const char *s)
{
        return _Generic(0 ? (s) : (void *) 1, const void *: 1, default: 2);
}

int
by_plain_pointee(char *s)
{
        return _Generic(0 ? (s) : (void *) 1, const void *: 1, default: 2);
}
