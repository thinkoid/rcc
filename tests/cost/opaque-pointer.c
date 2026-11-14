/* A pointer to a struct the program never opens is a pointer.  The
   struct is declared and nothing needs its object, so rcc takes it as
   a not-yet type (shape.md chapter 3), and a pointer to it is passed,
   returned and compared as any pointer is.

   On x86-64 the listing must be, byte for byte but for the .file line,
   the listing of this program with void * for every struct node *
   and without the struct's declaration.

   On aarch64 the listing must be the same twin's listing, byte for
   byte but for the .file line. */
struct node;

struct node *pass(struct node *p)
{
    return p;
}

int same(struct node *p, struct node *q)
{
    return p == q;
}
