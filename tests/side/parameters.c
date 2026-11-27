/* Functions of one to ten int parameters.  On x86-64 each of the first
   six takes the next slot below rbp and is stored there from its
   argument register before the body, movl %edi, -4(%rbp) to movl %r9d,
   -24(%rbp), read or not; the seventh and later have no slot and are
   read where the caller pushed them, 16(%rbp) and up, 8 bytes apart.
   On aarch64 every one takes a slot, from [x29, #16] up: the first
   eight are stored from w0 to w7, and the ninth and later are copied
   from above the frame through w17, after those stores, read or not,
   from [x29, #T] on, T the frame's size. */
int
one(int a)
{
        return a;
}

int
two(int a, int b)
{
        return a - b;
}

int
six(int a, int b, int c, int d, int e, int f)
{
        return f + e + d + c + b + a;
}

int
seven(int a, int b, int c, int d, int e, int f, int g)
{
        return g - a;
}

int
eight(int a, int b, int c, int d, int e, int f, int g, int h)
{
        return g + h;
}

int
nine(int a, int b, int c, int d, int e, int f, int g, int h, int i)
{
        return i;
}

int
ten(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
        return j * i;
}

int
unread(int a, int b)
{
        return 0;
}

int
nine_unread(int a, int b, int c, int d, int e, int f, int g, int h, int i)
{
        return 1;
}

void
void_unread(int a)
{
}
