/* A union with a double member, as a shell sizes its blocks by: the
   union is laid out, the double at its size and alignment, so its size
   is taken, and its long member is read.  A floating object is not
   yet, so the read of the double member is refused by its type's
   words. */
union align
{
        long l;
        double d;
        void *p;
};

long
size(void)
{
        return sizeof(union align);
}

long
whole(union align *a)
{
        return a->l;
}

int
fraction(union align *a)
{
        return a->d > 0;
}
