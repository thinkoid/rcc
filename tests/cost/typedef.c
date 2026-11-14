/* A typedef costs nothing.  A typedef name is another spelling of its
   type, and the tree keeps the type, not the spelling (shape.md
   chapter 3), so a local, a parameter, a cast and a sizeof written
   through one compile to what they compile to with the type spelled
   out.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with unsigned long for count_t, char *
   for text_t and int r[4] for row_t r.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
typedef unsigned long count_t;
typedef char *text_t;
typedef int row_t[4];

count_t length(text_t s)
{
    count_t n = 0;
    while (s[n])
        n++;
    return n;
}

int sum(int a)
{
    row_t r;
    text_t t = (text_t) &r[0];
    r[0] = a;
    r[1] = (int) sizeof (text_t);
    return r[0] + r[1] + (int) (count_t) t[0];
}
