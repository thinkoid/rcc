/* An array declared without its size is incomplete until a later
   declaration gives the size, 6.2.7p5, and sizeof takes no incomplete
   type, 6.5.4.4p1: the definition after the function comes too late
   for it. */
extern int table[];

int
size(void)
{
        return sizeof table;
}

int table[3];

int
main(void)
{
        return size();
}
