/* Chapter 3, pointer casts: ptrtoint and inttoptr at 64 bits are no
   instruction, an int is widened first.  The listing must show
   to_long() and from_long() as a mov to the slot and a mov back,
   nothing between; and from_int() widening the int with cltq, the
   sext of the int to 64 bits before the inttoptr. */
long to_long(int *p)
{
    return (long) p;
}

int *from_long(long l)
{
    return (int *) l;
}

int *from_int(int i)
{
    return (int *) i;
}
